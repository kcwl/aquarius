// aquarius_protocol.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "file_parse.h"
#include <iomanip>
#include <iostream>
#include <filesystem>
#include "cpp_generater.h"
#include "cpp_db_generate.h"
#include "model_parse.h"

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
		else if (std::string(argv[i]) == "--db_input")
		{
			while (i + 1 < argc && argv[i + 1][0] != '-')
			{
				std::string file = argv[++i];

				if (!check_suffix(file, "tbl"))
				{
					std::cout << "Input file error! there must be *.tbl file!\n";
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
		std::filesystem::path ofs_path(output_path);

		std::filesystem::path files(file);

		ofs_path = ofs_path.append(files.filename().string());

		auto extension = std::filesystem::path(file).extension();
		if (extension == ".virgo")
		{
			proto_parser pr{};

			if (!pr.read_file(file))
				return 0;

			

			if (output_type == "cpp")
			{
				std::ofstream ofs_h(ofs_path.string() + ".h");

				std::ofstream ofs_cpp(ofs_path.string() + ".cpp");

				ofs_h << "#pragma once\n";
				ofs_h << "#include <aquarius.hpp>\n";

				ofs_cpp << "#include \"" << files.filename().string() + ".h\"\n";

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
					if (k->type() == struct_type::structure)
					{
						cpp::data_field_generator(ofs_h, k, false).generate_stream_define();
					}
					else if (k->type() == struct_type::message)
					{
						auto msg = std::static_pointer_cast<message_field>(k);
						if (!msg)
							continue;

						cpp::data_field_generator(ofs_h, msg->request(), false).generate_stream_define();

						cpp::data_field_generator(ofs_h, msg->response(), false).generate_stream_define();
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

				for (auto& k : pr.fields_)
				{
					if (k->type() == struct_type::structure)
					{
						cpp::data_field_generator(ofs_cpp, k, false).generate_stream_src();
					}
					else if (k->type() == struct_type::message)
					{
						auto msg = std::static_pointer_cast<message_field>(k);
						if (!msg)
							continue;

						cpp::data_field_generator(ofs_cpp, msg->request(), false).generate_stream_src();

						cpp::data_field_generator(ofs_cpp, msg->response(), false).generate_stream_src();
					}
				}
			}
		}
		else if (extension == ".tbl")
		{
			auto parse = std::make_shared<model_parse>();

			if (!parse->read_file(file))
				return 0;

			std::ofstream ofs(ofs_path.string() + ".h");

			cpp_db_generate generate{};

			ofs << "#include <aquarius/tbl.hpp>\n" << std::endl;

			for (auto& m : parse->models_)
			{
				generate.generate_table(ofs, m);
			}
		}
	}

	return 0;
}