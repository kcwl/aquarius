#include "idle_gen_cpp.h"

#include "common.hpp"
#include "file_descriptor.h"

#include <array>
#include <filesystem>
#include <string_view>

namespace
{
	const std::map<std::string, std::string> type_pair = {
		{ "int32", "int32_t" },	  { "int64", "int64_t" },	  { "string", "std::string" }, { "bool", "bool" },
		{ "uint32", "uint32_t" }, { "uint64", "uint64_t" },	  { "bytes", "bytes" },		   { "float", "float" },
		{ "double", "double" },	  { "fixed32", "fixed32_t" }, { "fixed64", "fixed64_t" },
	};

	std::string get_type_name(const std::string& type, const std::string& sub_type = {})
	{
		std::string temp_type = type;

		temp_type == "repeated" ? temp_type = sub_type : std::string{};

		auto iter = type_pair.find(temp_type);

		if (iter == type_pair.end())
			return {};

		std::string result{};

		type == "repeated" ? result = std::string("std::vector<") + sub_type + ">" : result = iter->second;

		return result;
	}

} // namespace

namespace elastic
{
	namespace compiler
	{
		namespace cpp
		{
			bool generate_cpp::generate(file_descriptor* input_file, const std::string& output_dir)
			{
				input_file_ptr_ = input_file;

				output_file_name_ = output_dir;

				write_h_stream_.open(input_file_ptr_->file_name() + ".mpr.h", std::ios::binary | std::ios::out);

				if (!write_h_stream_.is_open())
					return false;

				write_cpp_stream_.open(input_file_ptr_->file_name() + ".mpr.cpp", std::ios::binary | std::ios::out);

				if (!write_cpp_stream_.is_open())
					return false;

				if (!write_file())
					return false;

				return true;
			}

			bool generate_cpp::write_file()
			{
				write_struct_declare();

				real_write_file(write_h_stream_);

				write_struct_define();

				real_write_file(write_cpp_stream_);

				return true;
			}

			void generate_cpp::write_struct_declare()
			{
				write_struct_declare_header();

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

						write_construct(s.name_);

						write_internal_func_declare();

						write_members(s.structs_);

						end_write_class();
					}
					else
					{
						lines.push_back(s.note_.content_);
					}
				}

				if (has_namespace)
				{
					end_write_package();
				}
			}

			void generate_cpp::write_struct_declare_header()
			{
				pragma("once");
				include_file("aquarius/elastic.hpp");
				line_feed();
			}

			void generate_cpp::write_struct_define()
			{
				lines.clear();

				include_file(input_file_ptr_->file_name() + ".mpr.h", false);
				line_feed();

				bool has_namespace = false;

				for (auto& s : input_file_ptr_->structs())
				{
					if (s.type_ == "package")
					{
						begin_write_package(s.name_);

						has_namespace = true;
					}
					else if (s.type_ == "message")
					{
						write_internal_func_def(s, "from_binary");

						write_internal_func_def(s, "to_binary");
					}
				}

				if (has_namespace)
				{
					end_write_package();
				}
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
				auto base_type = std::format("aquarius::message_lite");

				lines.push_back("class " + rs.name_ + " final : public " + base_type + rs.note_.content_);

				lines.push_back("{");
			}

			void generate_cpp::write_construct(const std::string& class_name)
			{
				lines.push_back("public:");
				lines.push_back(class_name + "() = default;");
				lines.push_back("virtual ~" + class_name + "() = default;");
				line_feed();
			}

			void generate_cpp::write_internal_func_declare()
			{
				lines.push_back("private:");
				write_parse_func("from_binary");
				write_parse_func("to_binary");
			}

			void generate_cpp::write_parse_func(const std::string& func_name)
			{
				lines.push_back("virtual bool internal_" + func_name + "(aquarius::flex_buffer_t& buffer) final;");
				line_feed();
			}

			void generate_cpp::write_members(const std::vector<reflactor_structure>& rss)
			{
				lines.push_back("public:");

				for (auto& rs : rss)
				{
					auto type = get_type_name(rs.type_, rs.sub_type_);

					if (type.empty())
						continue;

					lines.push_back(type + " " + rs.name_ + ";" + rs.note_.content_);
				}
			}

			void generate_cpp::end_write_class()
			{
				lines.push_back("};");
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
						last_space = space;
						space += tab;
					}
				}

				ofs.flush();
				ofs.close();
			}

			void generate_cpp::write_internal_func_def(const reflactor_structure& rs, const std::string& func_name)
			{
				!rs.structs_.empty() ? lines.push_back("bool " + rs.name_ + "::internal_" + func_name +
													   "(aquarius::flex_buffer_t& buffer)")
									 : lines.push_back("bool " + rs.name_ + "::internal_" + func_name +
													   "(aquarius::flex_buffer_t&)");
				lines.push_back("{");

				for (auto& mem : rs.structs_)
				{
					if (mem.type_.empty())
						continue;

					auto type = get_type_name(mem.type_, mem.sub_type_);

					if (type.empty())
						continue;

					lines.push_back("if (!aquarius::" + func_name + "(" + mem.name_ + ", buffer))");
					lines.push_back("{");
					lines.push_back("return false;");
					lines.push_back("}");
					line_feed();
				}

				lines.push_back("return true;");
				lines.push_back("}");
			}

			void generate_cpp::pragma(const std::string& name)
			{
				lines.push_back("#pragma " + name);
			}

			void generate_cpp::include_file(const std::string& file_name, bool has_libary)
			{
				has_libary ? lines.push_back("#include <" + file_name + ">")
						   : lines.push_back("#include \"" + file_name + "\"");
				line_feed();
			}

			void generate_cpp::line_feed()
			{
				lines.push_back({});
			}

		} // namespace cpp
	}	  // namespace compiler
} // namespace elastic