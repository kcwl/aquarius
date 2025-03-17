#pragma once
#include "file_descripe.hpp"
#include <deque>
#include <filesystem>
#include <fstream>
#include <map>
#include <utility>

constexpr static int regist_space = 5;

constexpr std::string_view message_alias = "_body_"sv;

struct format_space
{
	std::vector<char> format;
};

std::ostream& operator<<(std::ostream& os, const format_space& other)
{
	for (auto& c : other.format)
	{
		os << c;
	}

	return os;
}

class file_outer
{
public:
	file_outer(const command_line& line, file_descriper& fd)
		: fd_(fd)
	{
		input_file_name_ = line.input_file_name_ + ".proto." + line.output_type_;

		out_header_file_content_.open(input_file_name_, std::ios::out | std::ios::binary);

		std::filesystem::path file_path = std::filesystem::current_path();

		if (line.output_file_directory_ != ".")
			file_path.append(line.output_file_directory_);

		file_path.append(line.input_file_name_ + ".proto." + line.output_source_type_);

		out_filename_ = file_path.string();

		out_sources_file_content_.open(out_filename_, std::ios::out | std::ios::binary);

		struct_invokes_.emplace("namespace", [&](const message_info& info) { return this->out_namespace_begin(info); });
		struct_invokes_.emplace("import", [&](const message_info& info) { return this->out_import(info); });
		struct_invokes_.emplace("message",
								[&](const message_info& info)
								{
									this->outer_someone(info.netests);

									this->out_message(info);
								});
		struct_invokes_.emplace("enum", [&](const message_info& info) { return this->out_enum(info); });
	}

	void outer(const std::vector<message_info>& message_infos)
	{
		if (message_infos.empty())
			return;

		source_prefix();

		const auto& front = message_infos.front();

		if (front.type == "syntax")
			outer_syntax(front);

		outer_someone(message_infos);

		auto& back = header_lines_.back();

		if (back.empty())
			header_lines_.pop_back();

		out_regist();

		out_namespace_end();

		real_outer(out_header_file_content_, header_lines_);

		real_outer(out_sources_file_content_, source_lines_);
	}

private:
	void outer_someone(const std::vector<message_info>& message_infos)
	{
		if (message_infos.empty())
			return;

		for (auto& mi : message_infos)
		{
			auto iter = struct_invokes_.find(mi.type);

			if (iter == struct_invokes_.end())
				continue;

			struct_invokes_[mi.type](mi);
		}
	}

	void outer_syntax(const message_info& message_info)
	{
		if (message_info.type_name == protocol_syntax)
		{
			header_lines_.push_back("#include <aquarius.hpp>");
		}
	}

	void out_namespace_begin(const message_info& mi)
	{
		header_line_enter();
		std::string namespace_str = mi.type + " " + mi.type_name;
		header_lines_.push_back(namespace_str);
		header_lines_.push_back("{");

		namespaces_.push_back("}");

		source_lines_.push_back(namespace_str);
		source_lines_.push_back("{");
	}

	void out_namespace_end()
	{
		for (auto& n : namespaces_)
		{
			header_lines_.push_back(n);
			source_lines_.push_back(n);
		}
	}

	void out_import(const message_info& mi)
	{
		header_lines_.push_back("#include \"" + mi.type_name + "\"");
		header_line_enter();
		header_line_enter();
	}

	void out_message(const message_info& mi)
	{
		auto class_name = mi.type_name;
		if (!mi.special_key.empty())
		{
			if (fd_.syntax() != protocol_syntax)
				return;

			class_name.append("_body");
			class_name.append("_" + mi.special_key);
			registers_.emplace(mi.specila_number, mi.type_name);
		}

		header_lines_.push_back("class " + class_name + " final");
		header_lines_.push_back("{");
		header_lines_.push_back("public:");
		header_lines_.push_back(class_name + "() = default;");
		header_lines_.push_back("~" + class_name + "() = default;");
		header_line_enter();

		// copy construction
		header_lines_.push_back("public:");
		std::string function_str = class_name + "(const " + class_name + "& other)";
		header_lines_.push_back(function_str + ";");
		header_line_enter();

		source_lines_.push_back(class_name + "::" + function_str);
		bool is_first = true;
		for (auto& mem : mi.keys)
		{
			if (!fd_.exist(mem.type))
				return;

			if (is_first)
			{
				source_lines_.push_back(": " + mem.type_name + "(other." + mem.type_name + ")");

				is_first = !is_first;
			}
			else
			{
				source_lines_.push_back(", " + mem.type_name + "(other." + mem.type_name + ")");
			}
		}

		auto& back = source_lines_.back();
		back += "{}";
		source_line_enter();

		// move construction
		is_first = true;
		function_str = class_name + "(" + class_name + "&& other) noexcept";
		header_lines_.push_back(function_str + ";");
		header_line_enter();

		source_lines_.push_back(class_name + "::" + function_str);

		for (auto& mem : mi.keys)
		{
			std::string sp{};
			std::string move_left{};

			if (is_first)
			{
				sp = ":";
				is_first = !is_first;
			}
			else
			{
				sp = ",";
			}

			is_pod(mem.type_name) ? move_left = "std::move(" : move_left;

			source_lines_.push_back(sp + " " + mem.type_name + "(" + move_left + "other." + mem.type_name +
									(!move_left.empty() ? "))" : ")"));
		}

		source_lines_.push_back("{");
		source_lines_.push_back("other.clear();");
		source_lines_.push_back("}");
		source_line_enter();

		// operator= &
		header_lines_.push_back(class_name + "& operator=(const " + class_name + "& other);");
		header_line_enter();

		source_lines_.push_back(class_name + "& " + class_name + "::operator=(const " + class_name + "& other)");
		source_lines_.push_back("{");
		source_lines_.push_back("if(this != std::addressof(other))");
		source_lines_.push_back("{");

		for (auto& mem : mi.keys)
		{
			source_lines_.push_back(mem.type_name + " = other." + mem.type_name + ";");
		}

		source_lines_.push_back("}");

		source_line_enter();
		source_lines_.push_back("return *this;");
		source_lines_.push_back("}");
		source_line_enter();

		// operator= &&
		header_lines_.push_back(class_name + "& operator=(" + class_name + "&& other) noexcept;");
		header_line_enter();
		source_lines_.push_back(class_name + "& " + class_name + "::operator=(" + class_name + "&& other) noexcept");
		source_lines_.push_back("{");
		source_lines_.push_back(class_name + "{ std::move(other) }.swap(*this);");
		source_line_enter();
		source_lines_.push_back("return *this;");
		source_lines_.push_back("}");

		source_line_enter();

		header_lines_.push_back("public:");
		// from_binary
		header_lines_.push_back("bool from_binary(aquarius::flex_buffer_t& stream);");
		header_line_enter();

		source_lines_.push_back("bool " + class_name + "::from_binary(aquarius::flex_buffer_t & stream)");
		source_lines_.push_back("{");
		source_lines_.push_back("bool result = true;");
		for (auto& mem : mi.keys)
		{
			source_lines_.push_back("result &= aquarius::from_binary(" + mem.type_name + ", stream);");
		}
		source_line_enter();
		source_lines_.push_back("return result;");
		source_lines_.push_back("}");
		source_line_enter();

		// to_binary
		header_lines_.push_back("bool to_binary(aquarius::flex_buffer_t& stream);");
		header_line_enter();

		source_lines_.push_back("bool " + class_name + "::to_binary(aquarius::flex_buffer_t & stream)");
		source_lines_.push_back("{");
		source_lines_.push_back("bool result = true;");
		for (auto& mem : mi.keys)
		{
			source_lines_.push_back("result &= aquarius::to_binary(" + mem.type_name + ", stream);");
		}
		source_line_enter();
		source_lines_.push_back("return result;");
		source_lines_.push_back("}");
		source_line_enter();

		// swap
		header_lines_.push_back("void swap(" + class_name + "& other);");
		header_line_enter();
		source_lines_.push_back("void " + class_name + "::swap(" + class_name + "& other)");
		source_lines_.push_back("{");

		for (auto& mem : mi.keys)
		{
			if (is_pod(mem.type))
				source_lines_.push_back("std::swap(" + mem.type_name + ", other." + mem.type_name + ");");
			else
				source_lines_.push_back(mem.type_name + ".swap(other." + mem.type_name + ");");
		}

		source_lines_.push_back("}");
		source_line_enter();

		// clear
		header_lines_.push_back("void clear();");
		header_line_enter();

		source_lines_.push_back("void " + class_name + "::clear()");
		source_lines_.push_back("{");

		for (auto& mem : mi.keys)
		{
			if (is_pod(mem.type))
			{
				std::string value = "0";
				if (mem.type == "float")
					value += ".f";
				else if (mem.type == "double")
					value += ".0";

				source_lines_.push_back(mem.type_name + " = " + value + ";");
			}
		}

		source_lines_.push_back("}");

		// header_lines_.push_back("private:");
		// header_lines_.push_back("friend class aquarius::access;");
		// header_line_enter();
		// header_lines_.push_back("template<typename _Archive>");
		// header_lines_.push_back("void serialize(_Archive& ar)");
		// header_lines_.push_back("{");

		// for (auto& mi : mi.keys)
		//{
		//	header_lines_.push_back("ar& " + mi.type_name + ";");
		// }

		// header_lines_.push_back("}");

		// header_line_enter();

		// member varant
		header_lines_.push_back("public:");
		for (auto& mem : mi.keys)
		{
			if (!check_complex_type(mem))
			{
				header_lines_.push_back(mem.type + " " + mem.type_name + ";");
				header_line_enter();
			}
		}

		auto& back_line = header_lines_.back();
		back_line = "};";
		header_line_enter();
	}

	void out_enum(const message_info& mi)
	{
		header_lines_.push_back("enum class " + mi.type_name);
		header_lines_.push_back("{");

		for (auto& em : mi.keys)
		{
			header_lines_.push_back(em.type + " " + em.type_name + ",");
		}

		auto& line = header_lines_.back();
		line = line.substr(0, line.find_first_of(','));

		header_lines_.push_back("};");

		header_line_enter();
	}

	void out_regist()
	{
		if (fd_.syntax() != protocol_syntax)
			return;

		for (int i = 0; i < regist_space; ++i)
		{
			header_lines_.push_back(crlf.data());
		}

		for (const auto& [key, value] : registers_)
		{
			int64_t key_number = 0;
			std::stringstream ss;
			ss << key;
			ss >> key_number;

			std::string temp_alias{};

			std::string line = "using " + value + "_";

			key_number % 2 == 0 ? temp_alias = "request" : temp_alias = "response";
			line += temp_alias;

			line += " = aquarius::" + temp_alias + "<" + value + message_alias.data() + temp_alias + ", " + key + ">;";

			header_lines_.push_back(line);
		}
	}

	void header_line_enter()
	{
		header_lines_.push_back({});
	}

	void source_line_enter()
	{
		source_lines_.push_back({});
	}

	bool is_pod(const std::string& type)
	{
		auto iter = std::find(pod_type_array.begin(), pod_type_array.end(), type);

		return iter != pod_type_array.end();
	}

	void real_outer(std::fstream& ofs, const std::vector<std::string>& lines)
	{
		if (!ofs.is_open())
			return;

		format_space space{};

		format_space last_space{};

		for (auto& line : lines)
		{
			if (!line.empty())
			{
				if (line[0] == ')' || line[0] == '}')
					space.format.pop_back();

				if (line[0] == ':' || line[0] == ',')
					ofs << tab;
			}

			auto keyword = line.substr(0, line.find_first_of(' '));

			if (check_cpp_keyword(keyword))
			{
				last_space = space;
			}

			if (line.find("public:") != std::string::npos || line.find("private:") != std::string::npos ||
				line.find("protected:") != std::string::npos)
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
				space.format.push_back(tab);
			}
		}

		ofs.flush();
		ofs.close();
	}
	bool check_cpp_keyword(const std::string& keyword)
	{
		return keyword == "namespace" || keyword == "class" || keyword == "enum";
	}

	bool check_complex_type(const key_value& value)
	{
		if (value.type.find("<") == std::string::npos)
			return false;

		auto temp_type = value.type.substr(0, value.type.find_first_of("<"));

		auto iter =
			std::find_if(complex_type_array.begin(), complex_type_array.end(), [&](auto t) { return temp_type == t; });

		if (iter == complex_type_array.end())
			return false;

		if (temp_type == "oneof")
		{
			header_lines_.push_back("std::variant" + value.type.substr(value.type.find_first_of("<")) + " " +
									value.type_name + ";");
		}
		else
		{
			header_lines_.push_back("std::" + value.type + " " + value.type_name + ";");
		}

		header_line_enter();

		return true;
	}

	void source_prefix()
	{
		source_lines_.push_back("#pragma once");
		source_lines_.push_back("#include \"" + input_file_name_ + "\"");
		source_line_enter();
	}

private:
	std::string input_file_name_;

	std::string out_filename_;

	std::vector<std::string> header_lines_;

	std::vector<std::string> source_lines_;

	std::fstream out_header_file_content_;

	std::fstream out_sources_file_content_;

	std::map<std::string, std::string> registers_;

	std::deque<std::string> namespaces_;

	std::string tab_;

	file_descriper& fd_;

	std::map<std::string, std::function<void(const message_info&)>> struct_invokes_;
};