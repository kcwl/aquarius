#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/context/auto_context.hpp>
#include <aquarius/flex_buffer.hpp>
#include <vector>

namespace aquarius
{
	namespace ip
	{
		class tcp;
	}
}

namespace aquarius
{
	template <typename T>
	struct basic_processor;

	template <>
	struct basic_processor<ip::tcp>
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

		static constexpr std::size_t package_limit = 4096 - 5;

		template <typename Session>
		auto read(flex_buffer buffer, std::shared_ptr<Session> session) -> awaitable<void>
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

				uint8_t mode{};
				complete_buffer.load(&mode, sizeof(mode));

				context::auto_context_invoke<Session>::apply(mode, complete_buffer, proto, session);
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

			auto total = (buffer_size + package_limit) / package_limit;

			if (total > (std::numeric_limits<uint8_t>::max)())
				return {};

			uint16_t flag = static_cast<uint8_t>(total) << 8;

			uint8_t counter = 0;

			while (true)
			{
				flag |= counter;

				auto cur_size = std::min<std::size_t>(buffer_size, package_limit);

				complete_buffers.push_back({});
				auto& write_buffer = complete_buffers.back();
				write_buffer.save((uint8_t*)&flag, sizeof(flag));
				write_buffer.save((uint8_t*)&proto, sizeof(uint32_t));
				write_buffer.save((uint8_t*)buffer.wdata(), cur_size);
				buffer.consume(cur_size);
				buffer_size -= cur_size;

				++counter;

				if (buffer_size == 0)
					break;
			}

			return complete_buffers;
		}

		std::map<std::size_t, package> buffers_;
	};
} // namespace aquarius