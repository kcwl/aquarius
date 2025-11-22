#pragma once
#include <fstream>
#include "file_parse.h"
#include <memory>

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			class data_field_generator
			{
			public:
				enum json_type
				{
					integer,
					string,
					array,
					object
				};
			public:
				data_field_generator(std::ofstream& ofs, std::shared_ptr<data_field> data_field_ptr, bool has_end = true);

				~data_field_generator();

			public:
				void generate();

				void generate_header();

				void generate_member_variable_define(char end);

				void scope_public();
				void scope_private();
				void scope_protected();

				void generate_equal_define();

				void generate_equal_src();

				void generate_stream_define();

				void generate_stream_src();

				void generate_to_tag(std::shared_ptr<data_field> data_ptr);

				void generate_from_tag(std::shared_ptr<data_field> data_ptr);

				void generate_json_from_define(std::shared_ptr<data_field> data_ptr);

				void generate_json_to_define(std::shared_ptr<data_field> data_ptr);

			private:
				bool generator_array_stream(const std::string& type, const std::string& name);

				void generator_normal_stream(const std::string& name);

				bool generate_to_int(const std::string& type, const std::string& value);
				bool generate_to_string(const std::string& type, const std::string& value);
				bool generate_to_array(const std::string& type, const std::string& value);
				bool generate_to_object(const std::string& type, const std::string& value);

				bool generate_from_int(const std::string& type, const std::string& value);
				bool generate_from_string(const std::string& type, const std::string& value);
				bool generate_from_array(const std::string& type, const std::string& value);
				bool generate_from_object(const std::string& type, const std::string& value);

				json_type check_type(const std::string& type);

			protected:
				std::ofstream& ofs_;

			private:
				std::shared_ptr<data_field> data_field_ptr_;

				bool has_end_;
			};

			class message_field_generator
			{
			
			public:
				message_field_generator(std::ofstream& ofs, std::shared_ptr<message_field> message_field_ptr);

			public:
				void generate();

				void generate_protocol_define(std::shared_ptr<data_field> data_field_ptr, const std::string& method);

				void generate_protocol_alias_define();

				void generate_protocol_src();

			private:
				void generate_inheritance_serialize_define(const std::string& method);

				void generate_serialize_method_define();

				void generate_construction_define(const std::string& name, bool has_member);

				void generate_proto_request_define();

				void generate_proto_response_define();

				void generate_construction_src(std::shared_ptr<data_field> data_ptr, bool has_member);

				void generate_serialize_method_src(std::shared_ptr<data_field> data_ptr);

			private:
				std::shared_ptr<message_field> message_field_ptr_;

				std::ofstream& ofs_;

				std::list<std::string> json_to_wait_;

				std::list<std::string> json_from_wait_;
			};
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius