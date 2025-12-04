#pragma once
#include "parser.h"
#include "data_field.h"

namespace aquarius
{
	namespace lazytool
	{

		class router_field : public data_field, public parser
		{
		public:
			router_field();
			~router_field() = default;

		public:
			virtual parser::parse_error visit(std::ifstream& ifs, std::size_t& column, std::size_t& row) override;

		public:
			void set_protocol(const std::string& protocol);

			std::string protocol() const;

			void set_method(const std::string& method);

			std::string method() const;

			void set_key(const std::string& key);

			std::string key() const;

		private:
			bool check_protocol_type(const std::string& protocol_type);

			bool check_mode(const std::string& protocol, const std::string& mode);

		private:
			std::string protocol_;

			std::string method_;

			std::string key_;
		};
	} // namespace lazytool
} // namespace aquarius