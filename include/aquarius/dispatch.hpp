#pragma once
#include "visitor.hpp"
#include "message/null_message.hpp"

namespace aquarius
{
	namespace dispatch
	{
		template<typename Request, typename Context>
		int accept(std::shared_ptr<Context> ctx_ptr, std::shared_ptr<Request> msg_ptr)
		{
			if (msg_ptr == nullptr)
				return 1;

			using request_type = visitor<Request, int>;

			auto request_ptr = std::dynamic_pointer_cast<request_type>(ctx_ptr);

			if (request_ptr == nullptr)
			{
				return ctx_ptr->visit(std::make_shared<null_message>(msg_ptr));
			}
				
			return request_ptr->visit(msg_ptr);
		}

		template<typename Request, typename Context>
		void serialize(std::shared_ptr<Request> msg_ptr, std::shared_ptr<Context> ctx_ptr, msg::header_value header, ftstream& buf)
		{
			//msg_ptr->copy(header);

			//msg_ptr->serialize(buf);

			dispatch::accept(ctx_ptr, msg_ptr);
		}
	}
}