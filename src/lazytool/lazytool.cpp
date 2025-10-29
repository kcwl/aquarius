// aquarius_protocol.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "file_parse.h"
#include <iomanip>
#include <iostream>
#include <filesystem>
#include "enumture_generater.h"
#include "message_generater.h"
#include "structure_generater.h"
#include "protocol_generater.h"

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

bool check_suffix(const std::string& file_path, const std::string& suffix)
{
	std::filesystem::path file(file_path);

	return file.extension().compare(suffix);
}

bool parse_command(int argc, char** argv, std::vector<std::string>& input_files, std::string& output_file,
				   std::string& output_state, std::string& standard)
{
	for (int i = 1; i < argc; ++i)
	{
		if (std::string(argv[i]) == "--help")
		{
			help();
			return false;
		}
		else if (std::string(argv[i]) == "--version")
		{
			std::cout << "lazytool version 0.1\n";
			return false;
		}
		else if (std::string(argv[i]) == "--input")
		{
			while (i + 1 < argc && argv[i + 1][0] != '-')
			{
				std::string file = argv[++i];

				if (!check_suffix(file, "virgo"))
				{
					std::cout << "Input file error! there must be *.virgo file!\n";
					return false;
				}

				input_files.push_back(file);
			}

			if (input_files.empty())
			{
				std::cout << "No input files!\n";
				return false;
			}
		}
		else if (std::string(argv[i]) == "--cpp_out")
		{
			if (i + 1 < argc && argv[i + 1][0] != '-')
			{
				output_file = argv[++i];
				output_state = "cpp";
			}
			else
			{
				std::cout << "No output file path!\n";
				return false;
			}
		}
		else if (std::string(argv[i]) == "--cpp_standard")
		{
			if (i + 1 < argc && argv[i + 1][0] != '-')
			{
				standard = argv[++i];
			}
			else
			{
				std::cout << "No C++ standard specified! use default c++20\n";
				standard = "c++20";
			}
		}
		else
		{
			std::cout << "Unknown option: " << argv[i] << "\n";
			return false;
		}
	}

	return true;
}

int main(int argc, char** args)
{
	if (argc < 2)
	{
		std::cout << "No input files!\n";
		return 0;
	}

	std::vector<std::string> input_files{};

	std::string output_path{};
	std::string output_type{};
	std::string standard{};

	if (!parse_command(argc, args, input_files, output_path, output_type, standard))
		return 0;

	if (output_path.empty())
	{
		std::cout << "No output file path, that will be explicit output type!\n";
		return 0;
	}

	for (auto& file : input_files)
	{
		file_parser pr{};

		if (!pr.read_file(file))
			return 0;

		std::filesystem::path ofs_path(output_path);

		std::filesystem::path files(file);

		ofs_path = ofs_path.append(files.filename().string());

		

		if (output_type == "cpp")
		{
			std::ofstream ofs_h(ofs_path.string() + ".h");

			std::ofstream ofs_cpp(ofs_path.string() + ".cpp");

			ofs_h << "#pragma once\n";
			ofs_h << "#include <aquarius.hpp>\n";

			ofs_cpp << "#include \"" << files.filename().string() + ".h\"\n";

			for (auto& k : pr.keywords_)
			{
				if (k->struct_type_ == struct_type::enumture)
				{
					cpp::enumture_generate().visit(k, ofs_h, ofs_cpp);
				}
				else if (k->struct_type_ == struct_type::structure)
				{
					cpp::structure_generate().visit(k, ofs_h, ofs_cpp);
				}
				else if (k->struct_type_ == struct_type::message)
				{
					cpp::message_generate().visit(k, ofs_h, ofs_cpp);
				}
				else if (k->struct_type_ == struct_type::protocol)
				{
					cpp::protocol_generate().visit(k, ofs_h, ofs_cpp);
				}
			}

			for (auto& k : pr.keywords_)
			{
				if (k->struct_type_ == struct_type::message)
				{
					cpp::message_generate().defined(k, ofs_h);
				}
				else if (k->struct_type_ == struct_type::protocol)
				{
					cpp::protocol_generate().defined(k, ofs_h);
				}
				
			}
		}
	}

	return 0;
}