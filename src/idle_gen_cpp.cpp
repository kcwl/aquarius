#include "idle_gen_cpp.h"

#include "common.hpp"
#include "file_descriptor.h"

#include <array>
#include <filesystem>
#include <iostream>
#include <string_view>

namespace
{
	const std::string generate_file_suffix = ".proto.hpp";

	std::string get_type_name(const std::string& type, const std::string& sub_type = {})
	{
		std::string temp_type = type;

		temp_type == "repeated" ? temp_type = sub_type : std::string{};

		auto iter = aquarius::key_type.find(temp_type);

		if (iter == aquarius::key_type.end())
		{
			std::string result{};

			type == "repeated" ? result = std::string("std::vector<") + sub_type + ">" : result = temp_type;

			return result;
		}

		return iter->second;
	}

	std::string get_keyword_name(const std::string& keyword)
	{
		auto iter = aquarius::key_word.find(keyword);

		if (iter == aquarius::key_word.end())
			return {};

		return iter->second;
	}

} // namespace

namespace aquarius
{
	namespace compiler
	{
		namespace cpp
		{
			bool generate_cpp::generate(file_descriptor* input_file, const std::string& output_dir)
			{
				input_file_ptr_ = input_file;

				output_file_name_ = output_dir;

				write_hpp_stream_.open(input_file_ptr_->file_name() + generate_file_suffix,
									   std::ios::binary | std::ios::out);

				if (!write_hpp_stream_.is_open())
					return false;

				write_struct_declare_header();

				write_struct_declare();

				real_write_file(write_hpp_stream_);

				return true;
			}

			void generate_cpp::write_struct_declare()
			{
				bool has_namespace = false;

				for (auto& s : input_file_ptr_->structs())
				{
					if (s.type_ == "package")
					{
						has_namespace = true;

						begin_write_package(s.name_);
					}
					else if (s.type_ == "message")
					{
						begin_write_class(s);

						write_members(s);

						write_swap_function(s);

						write_access_func(s);

						end_write_class();
					}
					else
					{
						// lines.push_back(s.note_.content_);
					}
				}

				write_message_register(input_file_ptr_->structs());

				auto& back = lines.back();

				if (back.empty())
					lines.pop_back();


				if (has_namespace)
				{
					end_write_package();
				}
			}

			void generate_cpp::write_struct_declare_header()
			{
				pragma("once");
				include_file("aquarius.hpp");

				line_feed();
			}

			void generate_cpp::begin_write_package(const std::string& name)
			{
				lines.push_back("namespace " + name);
				lines.push_back("{");
			}

			void generate_cpp::end_write_package()
			{
				lines.push_back("}");
			}

			void generate_cpp::begin_write_class(const reflactor_structure& rs)
			{
				lines.push_back(get_keyword_name(rs.type_) + " " + rs.name_);

				lines.push_back("{");
			}
			void generate_cpp::write_access_func(const reflactor_structure& rs)
			{
				if (rs.keywords_.empty())
					return;

				int count = 0;
				for (auto& mem : rs.keywords_)
				{
					if (mem.first == "string" || mem.first == "bytes")
					{
						count++;

						break;
					}
				}

				if (count == 0)
					return;

				lines.push_back("private:");
				lines.push_back("friend class elastic::access;");
				line_feed();

				lines.push_back("template<typename _Archive>");
				lines.push_back("void serialize(_Archive& ar)");
				lines.push_back("{");

				for (auto& mem : rs.keywords_)
				{
					lines.push_back("ar& " + mem.second + ";");
				}

				lines.push_back("}");
			}

			void generate_cpp::write_members(const reflactor_structure& srs)
			{
				if (srs.keywords_.empty())
					return;

				for (auto& rs : srs.keywords_)
				{
					auto type = get_type_name(rs.first);

					if (type.empty())
						continue;

					lines.push_back(type + " " + rs.second + ";");
				}

				line_feed();
			}

			void generate_cpp::write_swap_function(const reflactor_structure& rs)
			{
				if (rs.keywords_.empty())
					return;

				lines.push_back("void swap(" + rs.name_ + "& other)");
				lines.push_back("{");
				
				for (auto& m : rs.keywords_)
				{
					lines.push_back("std::swap(" + m.second + ", other." + m.second + ");");
				}
				
				lines.push_back("}");

				line_feed();
			}

			void generate_cpp::end_write_class()
			{
				lines.push_back("};");
				line_feed();
			}

			void generate_cpp::real_write_file(std::ofstream& ofs)
			{
				std::string space{};

				std::string last_space{};

				for (auto& line : lines)
				{
					if (!line.empty())
					{
						if (line[0] == ')' || line[0] == '}')
							space.pop_back();
					}

					if (line.size() > 6)
					{
						auto temp = line.substr(0, 6);

						if (temp == "struct")
							last_space = space;
					}

					if (line.contains("public:") || line.contains("private:") || line.contains("protected:"))
					{
						ofs << last_space;
					}
					else
					{
						ofs << space;
					}

					ofs << line << crlf;

					if (line[0] == '{' || line[0] == '(')
					{
						space += tab;
					}
				}

				ofs.flush();
				ofs.close();
			}

			void generate_cpp::pragma(const std::string& name)
			{
				lines.push_back("#pragma " + name);
			}

			void generate_cpp::include_file(const std::string& file_name, bool has_libary)
			{
				has_libary ? lines.push_back("#include <" + file_name + ">")
						   : lines.push_back("#include \"" + file_name + "\"");
			}

			void generate_cpp::line_feed()
			{
				lines.push_back({});
			}

			void generate_cpp::write_message_register(const std::vector<reflactor_structure>& rs)
			{
				line_feed();
				line_feed();
				line_feed();
				line_feed();

				for (auto& mem : rs)
				{
					if (mem.number_.empty())
						continue;

					auto pos = mem.name_.find_last_of('_');

					if (pos == std::string::npos)
					{
						continue;
					}

					auto suffix = mem.name_.substr(pos);

					pos = mem.name_.rfind('_', pos - 1);

					if (pos == std::string::npos)
						continue;

					suffix = mem.name_.substr(pos + 1);

					if (suffix == "body_request")
					{
						lines.push_back("using " + mem.name_.substr(0, pos) + mem.name_.substr(pos + 5) +
										" = aquarius::request<" + mem.name_ + ", " + mem.number_ + ">;");
					}

					if (suffix == "body_response")
					{
						lines.push_back("using " + mem.name_.substr(0, pos) + mem.name_.substr(pos + 5) +
										" = aquarius::response<" + mem.name_ + ", " + mem.number_ + ">;");
					}
				}
			}
		} // namespace cpp
	} // namespace compiler
} // namespace aquarius