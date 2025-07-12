#include "tcp_transfer.hpp"
#include "proto/pub_sub.proto.hpp"
#include "sdnode.h"

namespace serviced
{
	AQUARIUS_STREAM_CONTEXT(subscriber, rpc_channel_subscribe)
	{
		auto node = std::make_shared<sdnode>(this->request()->header()->uuid());

		tcp_transfer::get_mutable_instance().channel_subscribe(std::to_string(this->request()->body().topic), node);

		co_return aquarius::error_code{};
	}

	AQUARIUS_STREAM_CONTEXT(publish, rpc_channel_publish)
	{
		tcp_transfer::get_mutable_instance().publish(std::to_string(this->request()->body().topic), this->request()->body().channel_data);

		co_return aquarius::error_code{};
	}

	AQUARIUS_STREAM_CONTEXT(remove, rpc_channel_remove)
	{
		for (auto& t : this->request()->body().topics)
		{
			tcp_transfer::get_mutable_instance().channel_unsubscribe(std::to_string(t), this->request()->header()->uuid());
		}

		co_return aquarius::error_code{};
	}
} // namespace serviced
