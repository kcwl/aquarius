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

	const std::string file_suffix = ".mpr.hpp";

	std::string get_type_name(const std::string& type, const std::string& sub_type = {})
	{
		std::string temp_type = type;

		temp_type == "repeated" ? temp_type = sub_type : std::string{};

		auto iter = type_pair.find(temp_type);

		if (iter != type_pair.end())
		{
			return iter->second;
		}

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

				write_hpp_stream_.open(input_file_ptr_->file_name() + file_suffix, std::ios::binary | std::ios::out);

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

						write_access_func(s);

						write_members(s.structs_);

						end_write_class();
					}
					else
					{
						lines.push_back(s.note_.content_);
					}
				}

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
				include_file("aquarius/elastic.hpp");
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
				lines.push_back("class " + rs.name_);

				lines.push_back("{");
			}
			void generate_cpp::write_access_func(const reflactor_structure& rs)
			{
				if (rs.structs_.empty())
					return;

				int count = 0;
				for (auto& mem : rs.structs_)
				{
					if (mem.type_ == "string" || mem.type_ == "bytes" || !mem.sub_type_.empty())
					{
						count++;

						break;
					}
				}

				if (count == 0)
					return;

				lines.push_back("friend class aquarius::access;");
				line_feed();

				lines.push_back("template<typename _Archive>");
				lines.push_back("void serialize(_Archive& ar)");
				lines.push_back("{");

				for (auto& mem : rs.structs_)
				{
					lines.push_back("ar& " + mem.name_ + ";");
				}

				lines.push_back("}");
				line_feed();
			}

			void generate_cpp::write_members(const std::vector<reflactor_structure>& rss)
			{
				if (rss.empty())
					return;

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

					if (line.size() > 5)
					{
						auto temp = line.substr(0, 5);

						if (temp == "class")
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
				line_feed();
			}

			void generate_cpp::line_feed()
			{
				lines.push_back({});
			}

		} // namespace cpp
	}	  // namespace compiler
} // namespace elastic