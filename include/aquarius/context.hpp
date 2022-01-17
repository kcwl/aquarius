#pragma once
#include <memory>
#include "visitor.hpp"
#include "tcp/message.hpp"

namespace aquarius
{
	class session;
	 
	class context 
		: public visitor<null_request>
	{
	public:
		virtual int visit(std::shared_ptr<null_request>)
		{ 
			return 0; 
		};

		void attach_session(std::shared_ptr<session> session_ptr)
		{
			session_ptr_ = session_ptr;
		}

		virtual void send_response(int result)
		{
			result = 1;
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
		using request_t = Request;
		using response_t = Response;

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

			if(!handle())
				return 0;

			return 1;
		}

	protected:
		virtual bool handle() = 0;

		virtual void send_response(int result) override
		{
			//resp_.set_result(result);

			//resp_.to_bytes(buffer_);

			//buffer_.clear();

			//session_ptr_->async_send(buffer_);
			result = 1;
		}

	protected:
		std::shared_ptr<Request> request_ptr_;

		Response resp_;

		streambuf buffer_;
	};
}