#include "tcp_transfer.hpp"
#include <proto/pub_sub.proto.hpp>
#include "sdnode.h"

namespace serviced
{
	AQUARIUS_CONTEXT(subscriber, rpc_channel_subscribe)
	{
		auto node = std::make_shared<sdnode>(this->message()->uuid());

		tcp_transfer::get_mutable_instance().channel_subscribe(std::to_string(this->message()->body().topic), node);

		co_return aquarius::error_code{};
	}

	AQUARIUS_CONTEXT(publish, rpc_channel_publish)
	{
		tcp_transfer::get_mutable_instance().publish(std::to_string(this->message()->body().topic), this->message()->body().channel_data);

		co_return aquarius::error_code{};
	}

	AQUARIUS_CONTEXT(remove, rpc_channel_remove)
	{
		for (auto& t : this->message()->body().topics)
		{
			tcp_transfer::get_mutable_instance().channel_unsubscribe(std::to_string(t), this->message()->uuid());
		}

		co_return aquarius::error_code{};
	}
} // namespace serviced
