#include "player.h"
#include <aquarius/module/session_schedule.hpp>
#include "server.h"

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

		auto player::feedback(flex_buffer& buffer) ->awaitable<void>
		{
			auto session = co_await aquarius::mpc_find_session<server::session_type>(id_);

			session->async_send(buffer);

			complete();
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

	} // namespace serviced
} // namespace aquarius