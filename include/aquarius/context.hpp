#pragma once
#include <memory>
#include "visitor.hpp"
#include "async_control.hpp"
#include "detail/router.hpp"

namespace aquarius
{
	class basic_message;

	class connect;

	class context 
		: public visitor<basic_message>
		, public async_control<connect>
	{
	public:
		virtual int visit(std::shared_ptr<basic_message>)
		{
			return 0;
		}
	private:
		virtual void complete(streambuf& stream)
		{
			protocol_type proto_id{};

			std::memcpy(&proto_id, reinterpret_cast<void*>(stream.data()), sizeof(protocol_type));

			//处理message
			detail::router::instance().route_invoke("msg_" + std::to_string(proto_id), stream);
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