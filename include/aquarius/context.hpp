#pragma once
#include <memory>
#include "visitor.hpp"
#include "connect.hpp"

namespace aquarius
{
	class context 
		: public visitor<null_message>
	{
	public:
		virtual int visit(std::shared_ptr<null_message>)
		{
			return 0;
		}


		void attach_connect(std::shared_ptr<connect> conn_ptr)
		{
			conn_ptr_ = conn_ptr;
		}

		template<typename Response>
		void send_response(Response&& resp)
		{
			if (conn_ptr_ == nullptr)
				return;

			conn_ptr_->queue_packet(std::forward<Response>(resp));
		}

	public:
		std::shared_ptr<connect> conn_ptr_;
	};

	template<class Request, class Response>
	class handler
		: public context
		, public visitor<Request>
	{
	public:
		handler()
			: request_ptr_(new Request{})
			, response_ptr_(new Response{})
		{
		}

	public:
		virtual int visit(std::shared_ptr<Request> request_ptr)
		{
			request_ptr_ = request_ptr;

			if (!handle())
			{
				return 0;
			}
				
			return 1;
		}

		void send_result(int error)
		{
			return send_response(response_ptr_);
		}

	protected:
		virtual bool handle() = 0;

	protected:
		std::shared_ptr<Request> request_ptr_;

		std::shared_ptr<Response> response_ptr_;
	};
}