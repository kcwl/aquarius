#include "player.h"

namespace aquarius
{
	namespace serviced
	{
		player::player(std::size_t id)
			: id_(id)
			, task_total_(0)
			, weight_(0)
		{}

		std::size_t player::id() const
		{
			return id_;
		}

		auto player::feedback(flex_buffer& buffer) -> awaitable<flex_buffer>
		{
			flex_buffer feed_buffer{};

			if (protocol_ == 0)
			{
				feed_buffer = co_await tcp_client_ptr_->async_send(buffer);
			}
			else if (protocol_ == 1)
			{
				feed_buffer = co_await http_client_ptr_->async_send(buffer);
			}

			complete();

			co_return feed_buffer;
		}

		void player::complete()
		{
			task_total_--;
		}

		int32_t player::weight() const
		{
			return weight_;
		}

		void player::weight(int32_t w)
		{
			weight_ = w;
		}

		void player::set_addr(const std::string& ip_addr, int32_t port)
		{
			ip_addr_ = ip_addr;
			port_ = port;
		}

		void player::set_protocol(int32_t proto)
		{
			protocol_ = proto;
		}

		std::string player::ip_addr() const
		{
			return ip_addr_;
		}

		int32_t player::port() const
		{
			return port_;
		}

		int32_t player::protocol() const
		{
			return protocol_;
		}
	} // namespace serviced
} // namespace aquarius