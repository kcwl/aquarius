#include "channel_schedule.h"
#include "error.hpp"
#include "proto/channel.virgo.h"

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_HANDLER(subscribe_tcp_request, subscribe_tcp_response, ctx_subscribe)
		{
			auto& topic = request()->body().topic;

			auto player_ptr = std::make_shared<player>(session()->uuid());

			if (!player_ptr)
				co_return errc::no_player;

			auto result = co_await player_ptr->create_client(session()->get_executor());

			if (!result)
				co_return errc::create_client_error;

			co_await mpc_subscribe(topic, player_ptr);

			co_return errc::success;
		}

		AQUARIUS_HANDLER(transfer_tcp_request, transfer_tcp_response, ctx_transfer)
		{
			auto player_ptr = std::make_shared<player>(session()->uuid());

			if (!player_ptr)
				co_return errc::no_player;

			flex_buffer buf{};
			buf.sputn(request()->body().feedbuf.data(), request()->body().feedbuf.size());

			auto feedbuf = co_await mpc_publish(request()->body().topic, buf, request()->seq_number());

			response().seq_number(request()->seq_number());

			std::copy((char*)feedbuf.data().data(), (char*)feedbuf.data().data() + feedbuf.data().size(), std::back_inserter(response().body().feedbuf));

			co_return errc::success;
		}
	} // namespace serviced
} // namespace aquarius