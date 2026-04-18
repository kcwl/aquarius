#pragma once
#include "code_generator.h"
#include "field.h"
#include <set>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace lazytool
	{
		class cpp_generator : public code_generator
		{
			enum json_type
			{
				integer,
				string,
				array,
				object
			};

		public:
			cpp_generator() = default;
			virtual ~cpp_generator() = default;

		public:
			virtual bool run(std::fstream& header, std::fstream& source,
							 const std::vector<std::shared_ptr<field_base>>& fields,
							 const std::string& protocol) override;

		private:
			bool generate_message(std::fstream& header, std::fstream& source, std::shared_ptr<message_field> field_ptr,
								  const std::string& protocol);

			bool generate_normal_data(std::fstream& header, std::fstream& source, std::shared_ptr<field_base> field_ptr,
									  const std::string& suffix = {});

			bool generate_data_field_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
											const std::string& protocol);

			bool generate_header(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
								 const std::string& suffix = {});

			void generate_inheritance_serialize_define(std::fstream& ofs, const std::string& protocol);

			bool generate_construction_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			// bool generate_equal_define(std::fstream& ofs, std::shared_ptr<data_field> field_ptr);

			bool generate_serialize_method_define(std::fstream& ofs);

			bool generate_member_func_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			void generate_impl_member_variable_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			bool generate_member_variable_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr, char end);

			bool generate_protocol_alias_define(std::fstream& ofs, std::shared_ptr<message_field> field_ptr,
												const std::string& protocol);

			bool generate_request_alias_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
											   const std::string& message_name, const std::string protocol,
											   const std::string& router);

			bool generate_response_alias_define(std::fstream& ofs, std::shared_ptr<field_base> field_ptr,
												const std::string& message_name, const std::string protocol);

			bool scope_public(std::fstream& ofs);

			bool scope_private(std::fstream& ofs);

			bool genetate_source(std::fstream& ofs, std::shared_ptr<field_base> field_ptr, const std::string& protocol);

			bool generate_construction_src(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			// bool generate_equal_src(std::fstream& ofs, std::shared_ptr<data_field> field_ptr);

			bool generate_serialize_method_src(std::fstream& ofs, std::shared_ptr<field_base> field_ptr, const std::string& protocol);

			bool generate_member_func_src(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			void generate_to_tag(std::fstream& ofs, std::shared_ptr<field_base> parser, bool has_impl = false);

			void generate_from_tag(std::fstream& ofs, std::shared_ptr<field_base> parser, bool has_impl = false);

			void generate_json_from_define(std::fstream& ofs, const std::string& field_name);

			void generate_json_to_define(std::fstream& ofs, const std::string& field_name);

			bool generate_to_int(std::fstream& ofs, const std::string& type, const std::string& value,
								 bool has_impl = false);

			bool generate_to_string(std::fstream& ofs, const std::string& type, const std::string& value,
									bool has_impl = false);

			bool generate_to_array(std::fstream& ofs, const std::string& type, const std::string& value,
								   bool has_impl = false);

			bool generate_to_object(std::fstream& ofs, const std::string& type, const std::string& value,
									bool has_impl = false);

			bool generate_from_int(std::fstream& ofs, const std::string& type, const std::string& value,
								   bool has_impl = false);

			bool generate_from_string(std::fstream& ofs, const std::string& type, const std::string& value,
									  bool has_impl = false);

			bool generate_from_array(std::fstream& ofs, const std::string& type, const std::string& value,
									 bool has_impl = false);
			bool generate_from_object(std::fstream& ofs, const std::string& type, const std::string& value,
									  bool has_impl = false);

			json_type check_type(const std::string& type);

			bool generate_model(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			void generate_model_field(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			void generate_model_member_func(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);

			void generate_model_member_name_func(std::fstream& ofs, std::shared_ptr<field_base> field_ptr);
		};
	} // namespace lazytool
} // namespace aquarius