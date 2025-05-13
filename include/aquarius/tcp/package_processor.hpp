#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/package_processor.hpp>
#include <aquarius/tcp/auto_register.hpp>
#include <list>
#include <map>

namespace aquarius
{
	namespace tcp
	{
		enum class mvcc
		{
			header = 0x0,
			package = 0x1,
			complete = 0x2,
			normal = package | complete
		};

		struct sequence
		{
			struct block
			{
				std::size_t seq;

				flex_buffer_t buffer;
			};

			std::size_t total;

			bool has_complete;

			std::list<block> buffers;

			bool check_complete()
			{
				return total == buffers.size();
			}
		};
	} // namespace tcp
	template <>
	struct package_processor<boost::asio::ip::tcp>
	{
		static constexpr std::size_t package_limit = 4096;

		template <typename Session>
		void read(flex_buffer_t& buffer, std::shared_ptr<Session> session_ptr)
		{
			uint8_t flag{};
			buffer.load(&flag, 1);

			uint32_t req_id{};
			buffer.load((uint8_t*)&req_id, sizeof(uint32_t));

			std::size_t seq = flag >> 2;

			auto& pack = buffers[req_id];

			auto mv = static_cast<tcp::mvcc>(flag & 0x3);

			switch (mv)
			{
			case tcp::mvcc::header:
			case tcp::mvcc::normal:
				{
					pack.total = seq;
					pack.buffers.push_back(tcp::sequence::block{ 0, std::move(buffer) });
				}
				break;
			case tcp::mvcc::complete:
			case tcp::mvcc::package:
				{
					pack.buffers.push_back({ seq, buffer });
				}
				break;
			default:
				break;
			}

			if (mv == tcp::mvcc::normal || mv == tcp::mvcc::complete)
			{
				int counter = 0;

				flex_buffer_t complete_buffer;

				for (auto iter = pack.buffers.begin(); iter != pack.buffers.end();)
				{
					if (iter->seq == counter++)
					{
						complete_buffer.save(iter->buffer.wdata(), iter->buffer.size());

						iter++;
					}
					else
					{
						partical_insert_sort(pack.buffers, iter, pack.buffers.end(), counter);
					}
				}

				tcp::invoke_context(req_id, complete_buffer, session_ptr);

				buffers.erase(req_id);
			}
		}

		std::vector<flex_buffer_t> write(std::size_t proto, flex_buffer_t buffer)
		{
			std::vector<flex_buffer_t> complete_buffers{};

			auto buffer_size = buffer.size();

			uint8_t total = static_cast<uint8_t>(buffer_size / package_limit) + 1;

			uint8_t flag = (total << 2) | static_cast<uint8_t>(tcp::mvcc::header);

			int counter = 0;

			while (buffer_size != 0)
			{
				auto cur_size = std::min<std::size_t>(buffer_size, package_limit);

				if (counter != 0) [[unlikely]]
				{
					cur_size < buffer_size
						? flag = static_cast<uint8_t>(counter << 2) | static_cast<uint8_t>(tcp::mvcc::package)
						: flag = static_cast<uint8_t>(counter << 2) | static_cast<uint8_t>(tcp::mvcc::complete);
				}

				complete_buffers.push_back({});
				auto& write_buffer = complete_buffers.back();
				write_buffer.save((uint8_t*)&flag, 1);
				write_buffer.save((uint8_t*)&proto, sizeof(uint32_t));
				write_buffer.save((uint8_t*)buffer.wdata(), cur_size);
				buffer_size -= cur_size;

				++counter;
			}

			return complete_buffers;
		}

		template <typename Iterator>
		void partical_insert_sort(std::list<tcp::sequence::block>& buffer, Iterator begin, Iterator end,
								  std::size_t pred)
		{
			for (auto iter = begin; iter != end;)
			{
				if (iter->seq != pred)
					continue;

				buffer.insert(begin, std::move(*iter));
				buffer.erase(iter);
			}
		}

		std::map<std::size_t, tcp::sequence> buffers;
	};
} // namespace aquarius