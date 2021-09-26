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

			protocol_type proto_id{};

			std::memcpy(&proto_id, reinterpret_cast<void*>(stream.data()), sizeof(protocol_type));

			//´¥·¢ctx
			auto ctx_ptr = router::instance().route_func("ctx_" + std::to_string(proto_id));

			return complete(proto_id, stream, ctx_ptr);
		}

		virtual void complete(protocol_type proto_id, streambuf& stream, std::shared_ptr<context> ctx_ptr) = 0;

	public:
		template<typename Response>
		void send_response(Response&& resp)
		{
			conn_ptr_->async_write_some(resp);
		}


	private:
		std::shared_ptr<Connect> conn_ptr_;
	};
}
