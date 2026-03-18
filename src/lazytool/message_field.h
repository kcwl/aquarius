#pragma once
#include "data_field.h"
#include <memory>

namespace aquarius
{
	namespace lazytool
	{
		class message_field : public field
		{
		public:
			message_field(struct_type s);

			~message_field() = default;

		public:
			void set_router(const std::string& r);

			std::string router() const;

			void set_request(std::shared_ptr<data_field> request);

			std::shared_ptr<data_field> request();

			void set_response(std::shared_ptr<data_field> response);

			std::shared_ptr<data_field> response();

			std::string method() const;

			void method(const std::string& m);

			std::string protocol() const;

			void set_protocol(const std::string& protocol);

		private:
			std::shared_ptr<data_field> request_ptr_;

			std::shared_ptr<data_field> response_ptr_;

			std::string router_;

			std::string method_;

			std::string protocol_;
		};
	} // namespace lazytool
} // namespace aquarius