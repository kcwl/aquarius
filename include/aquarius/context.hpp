#pragma once
#include <memory>
#include "visitor.hpp"
#include "tcp/null_message.hpp"
#include "session.hpp"

namespace aquarius
{
	class context 
		: public visitor<null_message>
	{
	public:
		virtual int visit(std::shared_ptr<null_message> req)
		{
			return 0;
		}

		void attach_session(std::shared_ptr<session> session_ptr)
		{
			session_ptr_ = session_ptr;
		}

		template<typename Response>
		void send_response(Response&& resp)
		{
			session_ptr_->async_send(std::forward<Response>(resp));
		}

		void on_close()
		{
			return session_ptr_->on_close();
		}

		void on_connect()
		{
			return session_ptr_->on_connect();
		}

	public:
		std::shared_ptr<session> session_ptr_;
	};

	template<class Request, class Response>
	class handler
		: public context
		, public visitor<Request>
	{
	public:
		handler()
			: request_ptr_(new Request{})
			, resp_()
		{
		}

		virtual ~handler() = default;

	public:
		virtual int visit(std::shared_ptr<Request> request_ptr)
		{
			request_ptr_.swap(request_ptr);

			if (!handle())
			{
				on_close();

				return 0;
			}
				
			return 1;
		}

	protected:
		virtual bool handle() = 0;

	protected:
		std::shared_ptr<Request> request_ptr_;

		Response resp_;
	};
}