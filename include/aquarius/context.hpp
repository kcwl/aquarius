#pragma once
#include <memory>
#include "visitor.hpp"
#include "schedule.hpp"

namespace aquarius
{
	class basic_message;

	class connect;

	class context 
		: public visitor<basic_message>
	{
	public:
		virtual int visit(std::shared_ptr<FinalType> visited)
		{
			return 0;
		}
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
		virtual int visit(std::shared_ptr<Request> request)
		{
			request_ptr_.swap(request);

			if(!handle())
				return 0;

			return 1;
		}

		virtual int visit(std::shared_ptr<basic_message> msg_ptr)
		{
			return 1;
		}

	protected:
		virtual bool handle() = 0;

		bool send_response(int result)
		{
			resp_.set_result(result);

			send_response(resp_);

			return true;
		}

	protected:
		std::shared_ptr<Request> request_ptr_;

		Response resp_;
	};
}