#pragma once
#include <memory>
#include "visitor.hpp"
#include "common.hpp"

namespace aquarius
{
	class basic_message;

	class connect;

	class context 
		: public visitor<basic_message,send_response_t>
	{
	public:
		virtual int visit(std::shared_ptr<basic_message>, send_response_t&&) { return 0; };
	};

	template<class Request, class Response>
	class handler
		: public context
		, public visitor<Request, send_response_t>
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
		virtual int visit(std::shared_ptr<Request> request_ptr, send_response_t&& send_reponse)
		{
			request_ptr_.swap(request_ptr);

			send_response_func_.swap(send_reponse);

			if(!handle())
				return 0;

			return 1;
		}

	protected:
		virtual bool handle() = 0;

		void send_response(int result)
		{
			resp_.set_result(result);

			resp_.to_bytes(buffer_);

			send_response_func_(std::move(buffer_));

			buffer_.clear();
		}

	protected:
		std::shared_ptr<Request> request_ptr_;

		Response resp_;

		send_response_t send_response_func_;

		streambuf buffer_;
	};
}