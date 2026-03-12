#include "message_field.h"

namespace aquarius
{
	namespace lazytool
	{
		message_field::message_field(struct_type s)
			: field(s)
			, request_ptr_(new data_field(struct_type::message))
			, response_ptr_(new data_field(struct_type::message))
			, router_()
			, method_()
		{}

		void message_field::set_router(const std::string& r)
		{
			router_ = r;
		}

		std::string message_field::router() const
		{
			return router_;
		}

		void message_field::set_request(std::shared_ptr<data_field> request)
		{
			request_ptr_ = request;
		}

		std::shared_ptr<data_field> message_field::request()
		{
			return request_ptr_;
		}

		void message_field::set_response(std::shared_ptr<data_field> response)
		{
			response_ptr_ = response;
		}

		std::shared_ptr<data_field> message_field::response()
		{
			return response_ptr_;
		}

		std::string message_field::method() const
		{
			return method_;
		}

		void message_field::method(const std::string& m)
		{
			method_ = m;
		}

		std::string message_field::protocol() const
		{
			return protocol_;
		}

		void message_field::set_protocol(const std::string& protocol)
		{
			protocol_ = protocol;
		}
	} // namespace lazytool
} // namespace aquarius