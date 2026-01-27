// aquarius_protocol.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "cpp_db_generate.h"
#include "cpp_generater.h"
#include "file_parse.h"
#include "model_parse.h"
#include <aquarius/cmd_options.hpp>
#include <filesystem>
#include <iomanip>
#include <iostream>

using namespace aquarius::lazytool;

void help()
{
	std::cout << "Usage: lazytool\n";
	std::cout << "Options:\n";
	std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--help" << "Display this help message\n";
	std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--version" << "Display version information\n";
	std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--input <file> ..."
			  << "Parse <file>, multi files will make them spearate with space\n";
	std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--cpp_out <file_path>"
			  << "Generate C++ code and output on <file_path>\n";
	std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--cpp_standard <c++11|c++14|c++17|c++20>"
			  << "Specify the C++ standard\n";
}

template <typename... Args>
bool check_suffix(const std::string& file_path, Args&&... args)
{
	std::filesystem::path file(file_path);

	return ((file.extension().compare(args) == 0) || ...);
}

void load_command(aquarius::cmd_options& op, int argc, char** argv)
{
	op.add_option<std::string>("help", "Display this help message", "require");
	op.add_option<std::string>("version", "Display version information");
	op.add_option<std::string>("input", "the file that input to parse, options[.virgo|.tbl], "
										  "different files will be invoked by different tools, such as * .virgo will "
										  "be parsed by virgo tool and *.tbl will be parsed by tbl tool");
	op.add_option<std::string>("cpp_out", "the file that output to cpp");
	op.add_option<std::string>("cpp_standard", "Specify the C++ standard, such as <c++11|c++14|c++17|c++20>",
							   "c++20");

	op.load_options(argc, argv);
}

bool parse_command(int argc, char** argv, std::string& input_file, std::string& output_file, std::string& output_state,
				   std::string& standard)
{
	aquarius::cmd_options op("lazytool options:");

	load_command(op, argc, argv);

	if (!op.option<std::string>("help").empty())
	{
		help();
		return false;
	}

	if (!op.option<std::string>("version").empty())
	{
		std::cout << "lazytool version 0.1\n";
		return false;
	}

	input_file = op.option<std::string>("input");

	if (input_file.empty())
	{
		std::cout << "No input files!\n";
		return false;
	}

	if (check_suffix(input_file, "virgo", "tbl"))
	{
		std::cout << "Input file error! there must be *.virgo file!\n";
		return false;
	}

	output_file = op.option<std::string>("cpp_out");

	if (output_file.empty())
	{
		std::cout << "No output file specified\n";
		return false;
	}

	standard = op.option<std::string>("cpp_standard");

	return true;
}

int main(int argc, char** args)
{
	if (argc < 2)
	{
		std::cout << "no parameter input! please input --help for more information\n";
		return 0;
	}

	std::string input_file{};

	std::string output_path{};
	std::string output_type{};
	std::string standard{};

	if (!parse_command(argc, args, input_file, output_path, output_type, standard))
		return 0;

	auto file = std::filesystem::path(input_file);

	std::filesystem::path ofs_path(output_path);

	ofs_path = ofs_path.append(file.filename().string());

	auto extension = file.extension();
	if (extension == ".virgo")
	{
		proto_parser pr{};

		if (!pr.read_file(file.string()))
			return 0;

		if (output_type == "cpp")
		{
			std::ofstream ofs_h(ofs_path.string() + ".h");

			std::ofstream ofs_cpp(ofs_path.string() + ".cpp");

			ofs_h << "#pragma once\n";
			ofs_h << "#include <aquarius.hpp>\n";

			ofs_cpp << "#include \"" << file.filename().string() + ".h\"\n";

			for (auto& k : pr.fields_)
			{
				ofs_h << std::endl;

				if (k->type() == struct_type::message)
				{
					cpp::message_field_generator(ofs_h, std::static_pointer_cast<message_field>(k)).generate();
				}
				else
				{
					cpp::data_field_generator(ofs_h, k).generate();
				}
			}

			ofs_h << std::endl;

			for (auto& k : pr.fields_)
			{
				if (k->type() == struct_type::message)
				{
					auto msg = std::static_pointer_cast<message_field>(k);

					if (!msg)
						continue;

					cpp::data_field_generator(ofs_h, k, false).generate_json_from_define(msg->request());

					cpp::data_field_generator(ofs_h, k, false).generate_json_to_define(msg->request());

					cpp::data_field_generator(ofs_h, k, false).generate_json_from_define(msg->response());

					cpp::data_field_generator(ofs_h, k, false).generate_json_to_define(msg->response());
				}
				else if (k->type() == struct_type::structure)
				{
					cpp::data_field_generator(ofs_h, k, false).generate_json_from_define(k);
					cpp::data_field_generator(ofs_h, k, false).generate_json_to_define(k);
				}
			}

			ofs_h << std::endl;

			for (auto& k : pr.fields_)
			{
				if (k->type() == struct_type::message)
				{
					cpp::message_field_generator(ofs_h, std::static_pointer_cast<message_field>(k))
						.generate_protocol_alias_define();
				}
			}

			for (auto& k : pr.fields_)
			{
				ofs_cpp << std::endl;

				if (k->type() == struct_type::message)
				{
					cpp::message_field_generator(ofs_cpp, std::static_pointer_cast<message_field>(k))
						.generate_protocol_src();
				}
				else if (k->type() == struct_type::structure)
				{
					cpp::data_field_generator gen(ofs_cpp, k, false);
					gen.generate_equal_src();
				}
			}

			for (auto& k : pr.fields_)
			{
				if (k->type() == struct_type::message)
				{
					auto msg = std::static_pointer_cast<message_field>(k);

					if (!msg)
						continue;

					cpp::data_field_generator(ofs_cpp, k, false).generate_from_tag(msg->request());

					cpp::data_field_generator(ofs_cpp, k, false).generate_to_tag(msg->request());

					cpp::data_field_generator(ofs_cpp, k, false).generate_from_tag(msg->response());

					cpp::data_field_generator(ofs_cpp, k, false).generate_to_tag(msg->response());
				}
				else if (k->type() == struct_type::structure)
				{
					cpp::data_field_generator(ofs_cpp, k, false).generate_from_tag(k);

					cpp::data_field_generator(ofs_cpp, k, false).generate_to_tag(k);
				}
			}
		}
	}
	else if (extension == ".tbl")
	{
		auto parse = std::make_shared<model_parse>();

		if (!parse->read_file(file.string()))
			return 0;

		std::ofstream ofs(ofs_path.string() + ".h");

		cpp_db_generate generate{};

		ofs << "#include <aquarius/tbl.hpp>\n" << std::endl;

		ofs << "using namespace aquarius::tbl;\n\n";

		for (auto& m : parse->models_)
		{
			generate.generate_table(ofs, m);
		}
	}

	return 0;
}