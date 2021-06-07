#pragma once
#include <memory>
#include "visitor.hpp"
#include "detail/streambuf.hpp"
#include "connect.hpp"

namespace aquarius
{
	class basic_message;

	class context 
		: public visitor<basic_message>
	{
	};

	template<class Request, class Response>
	class handler 
		: public context
		, public visitor<Request,int>
	{
	public:
		handler()
			: request_ptr_(new Request{})
			, resp_()
		{
		}

		virtual ~handler() = default;

	public:
		virtual int visit(std::shared_ptr<Request> request, std::shared_ptr<connect> conn_ptr)
		{
			request_ptr_.swap(request);

			conn_ptr_ = conn_ptr;

			if(!handle())
				return 0;

			send_response(1);

			return 1;
		}

		virtual int visit(std::shared_ptr<basic_message> msg_ptr, std::shared_ptr<connect> conn_ptr)
		{
			return 1;
		}

	protected:
		virtual bool handle() = 0;

		virtual bool send_response(int result)
		{
			resp_.set_result(result);

			detail::streambuf buf;
			resp_.to_bytes(buf);

			conn_ptr_->async_write_some(buf);

			return true;
		}

	protected:
		std::shared_ptr<Request> request_ptr_;

		std::shared_ptr<connect> conn_ptr_;

		Response resp_;
	};
}