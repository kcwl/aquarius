#include "field.h"
#include <iostream>

namespace aquarius
{
	namespace lazytool
	{
		field_base::field_base(struct_type s, const std::string& name)
			: type_(s)
			, name_(name)
			, fields_()
		{}

		void field_base::set_name(const std::string& name)
		{
			name_ = name;
		}

		std::string field_base::name() const
		{
			return name_;
		}

		std::string& field_base::name()
		{
			return name_;
		}

		struct_type field_base::type() const
		{
			return type_;
		}

		std::vector<std::pair<std::string, std::string>> field_base::fields() const
		{
			return fields_;
		}

		bool field_base::add_field(const std::string& type, const std::string& value)
		{
			auto iter =
				std::find_if(fields_.begin(), fields_.end(), [&](const auto& field) { return field.second == value; });

			if (iter != fields_.end())
			{
				std::cout << "Already have this field" << std::endl;

				return false;
			}

			fields_.emplace_back(type, value);

			return false;
		}

		message_field::message_field(struct_type s, const std::string& name)
			: field_base(s, name)
			, router_()
		{
			
		}

		void message_field::set_router(const std::string& r)
		{
			router_ = r;
		}

		std::string message_field::router() const
		{
			return router_;
		}

		void message_field::set_protocol_name(const std::string& name)
		{
			messages_[tag_request] = std::make_shared<field_base>(struct_type::request, name + request_subfix);
			messages_[tag_response] = std::make_shared<field_base>(struct_type::response, name + response_subfix);
		}
	} // namespace lazytool
} // namespace aquarius