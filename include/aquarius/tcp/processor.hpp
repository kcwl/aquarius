#pragma once
#include <aquarius/processor.hpp>
#include <boost/asio.hpp>
#include <aquarius/detail/protocol.hpp>
#include <generator>

namespace aquarius
{
    struct package
    {
        std::size_t total;

        std::list<flex_buffer> generators;

        bool check_complete()
        {
            return total == generators.size();
        }
    };

    template <>
    struct processor<boost::asio::ip::tcp>
    {
        static constexpr std::size_t package_limit = 4096 - 5;

        auto read(flex_buffer buffer) -> boost::asio::awaitable<void>
        {
            uint32_t proto{};
            buffer.load((uint8_t*)&proto, sizeof(uint32_t));

            buffers_[proto].generators.push_back(read_detail(buffer, buffers_[proto].total));

            if (buffers_[proto].check_complete()) [[likely]]
            {

                auto& buf = buffers_[proto];

                flex_buffer complete_buffer((buf.total - 2) * 4096);

                for (auto& gen : buf.generators)
                {
                    complete_buffer.save(gen.wdata(), gen.size());
                }
            }

            co_return;
        }

        flex_buffer read_detail(flex_buffer& buffer, std::size_t& total)
        {
            uint16_t flag{};
            constexpr auto size = sizeof(flag);
            buffer.load((uint8_t*)&flag, size);

            total = flag >> 8;

            buffer.commit(-static_cast<int64_t>(size));

            return buffer;
        }

        std::vector<flex_buffer> write(std::size_t proto, flex_buffer& buffer)
        {
            std::vector<flex_buffer> complete_buffers{};

            auto buffer_size = buffer.size();

            uint8_t total = static_cast<uint8_t>((buffer_size + package_limit) / package_limit);

            uint8_t flag = total == 1 ? total << 2 | static_cast<uint8_t>(tcp::mvcc::normal)
                : total << 2 | static_cast<uint8_t>(tcp::mvcc::header);

            int counter = 0;

            while (true)
            {
                auto cur_size = std::min<std::size_t>(buffer_size, package_limit);

                complete_buffers.push_back({});
                auto& write_buffer = complete_buffers.back();
                write_buffer.save((uint8_t*)&flag, 1);
                write_buffer.save((uint8_t*)&proto, sizeof(uint32_t));
                write_buffer.save((uint8_t*)buffer.wdata(), cur_size);
                buffer.consume(cur_size);
                buffer_size -= cur_size;

                ++counter;

                if (buffer_size == 0)
                    break;

                buffer_size > package_limit
                    ? flag = static_cast<uint8_t>(counter << 2) | static_cast<uint8_t>(tcp::mvcc::package)
                    : flag = static_cast<uint8_t>(counter << 2) | static_cast<uint8_t>(tcp::mvcc::complete);
            }

            return complete_buffers;
        }

        std::map<std::size_t, package> buffers_;
    };
}