#pragma once
#include "common.hpp"
#include "router.hpp"

namespace aquarius
{
	class context;

	template<typename Connect>
	class basic_schedule
	{
	public:
		void process(std::shared_ptr<Connect> conn_ptr, streambuf& stream)
		{
			conn_ptr_ = conn_ptr;

			protocol_type proto_id = make_proto_id(stream);

			if (proto_id  == static_cast<protocol_type>(-1))
				return;

			//´¥·¢ctx
			auto ctx_ptr = router::instance().route_func("ctx_" + std::to_string(proto_id));

			complete(proto_id, stream, ctx_ptr);

			return stream.clear();
		}

		virtual void complete(protocol_type proto_id, streambuf& stream, std::shared_ptr<context> ctx_ptr) = 0;

	public:
		template<typename Response>
		void send_response(Response&& resp)
		{
			conn_ptr_->async_write_some(resp);
		}

	private:
		protocol_type make_proto_id(streambuf& stream)
		{
			protocol_type proto_id{};

			uint32_t length{};

			std::size_t stream_length = stream.size();

			stream >> proto_id >> length;

			if (length > stream_length)
				return static_cast<protocol_type>(-1);

			return proto_id;
		}

	private:
		std::shared_ptr<Connect> conn_ptr_;
	};
}
