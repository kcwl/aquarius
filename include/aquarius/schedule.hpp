#pragma once
#include <cstddef>
#include <future>
#include "basic_schedule.hpp"
#include "visitor.hpp"

namespace aquarius
{
	class connect;

	class schedule
		: public std::enable_shared_from_this<schedule>
		, public basic_schedule<connect>
		
	{
	public:
		virtual ~schedule()
		{
		}

		virtual void complete(protocol_type proto_id, streambuf& stream, std::shared_ptr<context> ctx_ptr)
		{
			//处理message
			return router::instance().route_invoke("msg_" + std::to_string(proto_id), stream, shared_from_this(), ctx_ptr);
		}

		template<typename Request, typename Context>
		int accept(std::shared_ptr<Request> msg_ptr, std::shared_ptr<Context> ctx_ptr)
		{
			using request_type = visitor<Request, int>;

			auto request_ptr = std::dynamic_pointer_cast<request_type>(ctx_ptr);

			if(request_ptr == nullptr)
				return ctx_ptr->visit(msg_ptr);

			return request_ptr->visit(msg_ptr);
		}
	};

	template<class Func>
	struct invoker
	{
		static inline void apply(const Func& func, streambuf& buf, std::shared_ptr<schedule> schedule_ptr, std::shared_ptr<context> ctx_ptr)
		{
			auto msg_ptr = func();

			if(msg_ptr == nullptr)
				return;

			msg_ptr->parse_bytes(buf);

			schedule_ptr->accept(msg_ptr, ctx_ptr);
		}
	};
}