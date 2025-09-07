#include "cpp_generate.h"

namespace virgo
{
	cpp_generator::cpp_generator(const std::string& file_ename, const std::string& standard)
		: namespace_queue_()
		, file_name_(file_ename)
		, standard_(standard)
	{
		if (!create_file_stream())
		{
			throw std::runtime_error("create generate file stream failed!");
		}
	}

	cpp_generator::~cpp_generator()
	{
		generate_namespace_end();
	}

	void cpp_generator::generate(statement* state)
	{
		if (!state)
			return;

		ofs_ << std::endl;

		auto type_str = state->type;

		if (type_str == "message")
		{
			generate_message(state);
		}
		else if (type_str == "enum")
		{
			generate_enum(state);
		}
		else if (type_str == "tcp" || type_str == "udp" || type_str == "http")
		{
			generate_rpc(state);
		}
		else if (type_str == "import")
		{
			generate_single(state);
		}
		else if (type_str == "namespace")
		{
			generate_namespace_begin(state);
		}
	}

	void cpp_generator::generate_header()
	{
		ofs_ << "#pragma once\n";
		ofs_ << "#include <virgo.hpp>\n";
	}

	bool cpp_generator::create_file_stream()
	{
		auto file_name = file_name_.substr(0, file_name_.find_last_of("."));
		if (file_name_.empty())
			return false;

		file_name += ".proto.hpp";

		ofs_.open(file_name, std::ios::out | std::ios::binary);
		if (!ofs_.is_open())
			return false;

		return true;
	}

	void cpp_generator::generate_namespace_begin(statement* state)
	{
		ofs_ << std::endl << "namespace " << state->type << " {";

		namespace_queue_.push('}');
	}

	void cpp_generator::generate_namespace_end()
	{
		while (!namespace_queue_.empty())
		{
			ofs_ << namespace_queue_.front();
			namespace_queue_.pop();
		}
	}

	void cpp_generator::generate_enum(statement* state)
	{
		if (standard_ != "c++03" || standard_ != "c++98")
		{
			ofs_ << std::endl << "enum class " << state->name;
		}
		else
		{
			ofs_ << std::endl << "enum " << state->name;
		}

		ofs_ << std::endl << "{";

		for (auto& s : state->states)
		{
			if (s.name.empty())
				continue;

			ofs_ << std::endl << '\t' << s.name << ",";
		}

		ofs_.seekp(-1, std::ios::cur);

		ofs_ << std::endl << "};";
	}

	void cpp_generator::generate_message(statement* state)
	{
		ofs_ << std::endl << "struct " << state->name;
		ofs_ << std::endl << "{" << std::endl;
		for (auto& s : state->states)
		{
			if (s.type.empty())
				continue;

			if (s.sub_type == "repeated")
			{
				ofs_ << "\tstd::vector<" << convert_type(s.type) << ">";
			}
			else
			{
				ofs_ << '\t' << convert_type(s.type);
			}

			ofs_ << " " << s.name << ";\n";
		}

		ofs_ << "};";
	}

	void cpp_generator::generate_single(statement* state)
	{
		if (state->type.empty())
			return;

		ofs_ << std::endl << '\t' << convert_type(state->type) << " " << state->name << ";\n";
	}

	void cpp_generator::generate_rpc(statement* state)
	{
		std::string protocol("tcp");

		if (state->type == "udp")
		{
			protocol = "udp";
		}
		else if (state->type == "http")
		{
			protocol = "http";
		}

		ofs_ << "struct rpc_" << state->name << std::endl;
		ofs_ << "{\n";
		ofs_ << "\tconstexpr static auto id = \"" << *state->number << "\"sv;\n";
		ofs_ << "\tusing request = virgo::" << protocol << "::request<" << state->name << ">;\n";
		ofs_ << "\tusing response = virgo::" << protocol << "::response<" << state->name << ">;\n";
		ofs_ << "};\n";
		ofs_ << std::endl;
	}

	std::string cpp_generator::convert_type(const std::string& type_str)
	{
		std::string result = type_str;
		if (type_str == "int32")
		{
			result = "int32_t";
		}
		else if (type_str == "int64")
		{
			result = "int64_t";
		}
		else if (type_str == "uint32")
		{
			result = "uint32_t";
		}
		else if (type_str == "uint64")
		{
			result = "uint64_t";
		}
		else if (type_str == "string")
		{
			result = "std::string";
		}
		else if(type_str == "bytes")
			result = "std::vector<char>";

		return result;
	}
} // namespace virgo