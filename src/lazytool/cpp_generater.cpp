#include "cpp_generater.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
			data_field_generator::data_field_generator(std::ofstream& ofs, std::shared_ptr<data_field> data_field_ptr, bool is_json, bool is_equal_operator)
				: data_field_ptr_(data_field_ptr)
				, is_json_(is_json)
				, is_equal_operator_(is_equal_operator)
				, is_stream_operator_(false)
				, ofs_(ofs)
			{
			}

			data_field_generator::~data_field_generator()
			{
				ofs_ << "};\n";
			}

			void data_field_generator::generate()
			{
				generate_header();
				ofs_ << "\n{\n";

				generate_member_variable();
			}

			void data_field_generator::generate_header()
			{
				switch (data_field_ptr_->type())
				{
					case struct_type::structure:
						ofs_ << "struct ";
						break;
					case struct_type::enumture:
						ofs_ << "enum ";
						break;
					case struct_type::message:
						ofs_ << "class ";
						break;
					default:
						return;
				}

				ofs_ << data_field_ptr_->name();
			}

			void data_field_generator::generate_member_variable()
			{
				ofs_ << "public:" << std::endl;
				for (auto& [type, name] : data_field_ptr_->fields())
				{
					ofs_ << "\t" << type << " " << name << ";\n";
				}
			}

			message_field_generator::message_field_generator(std::ofstream& ofs, std::shared_ptr<message_field> message_field_ptr)
				: message_field_ptr_(message_field_ptr)
				, ofs_(ofs)
			{
			}

			void message_field_generator::generate()
			{
				generate_request();

				generate_response();
			}

			void message_field_generator::generate_request()
			{
				message_field_ptr_->request()->set_name(message_field_ptr_->name() + "_req_body");

				data_field_generator gen(ofs_, message_field_ptr_->request(), message_field_ptr_->method() == "http", true);

				gen.generate_header();

				ofs_ << "\n{\n";

				gen.generate_member_variable();
			}

			void message_field_generator::generate_response()
			{
				message_field_ptr_->response()->set_name(message_field_ptr_->name() + "_resp_body");

				data_field_generator gen(ofs_, message_field_ptr_->response(), message_field_ptr_->method() == "http", true);

				gen.generate_header();

				ofs_ << "\n{\n";

				gen.generate_member_variable();
			}

			void message_field_generator::generate_proto_define()
			{

			}

			void message_field_generator::generate_inheritance_serialize()
			{
			}

			void message_field_generator::generate_serialize_method()
			{
			}

			void message_field_generator::generate_construction(const std::string& name)
			{

			}

			//bool generater::generator_array_stream(std::ofstream& ofs, const std::string& type, const std::string& name)
			//{
			//	if (!type.contains("bytes"))
			//		return false;

			//	ofs << "\"[\";\n";
			//	ofs << "\tfor (auto& s : other." << name << ")\n";
			//	ofs << "\t{\n";
			//	ofs << "\t\tos << s << \", \";\n";
			//	ofs << "\t}\n";

			//	ofs << "\tos.seekp(-2, std::ios::cur);\n";
			//	ofs << "\tos << \"]\";\n";

			//	return true;
			//}

			//void generater::generator_normal_stream(std::ofstream& ofs, const std::string& name)
			//{
			//	ofs << "other." << name;
			//}
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius