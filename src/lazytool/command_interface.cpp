#include "command_interface.h"
#include "protocol_schema.h"
#include <iostream>

namespace aquarius
{
	namespace lazytool
	{
		command_interface::command_interface()
			: generators_()
			, schemas_()
			, input_files_()
			, output_file_path_()
			, input_dir_()
			, protocol_()
		{
			schemas_.insert({ __virgo__, std::make_shared<protocol_schema>() });
		}

		void command_interface::regist(const std::string& command, code_generator* generator)
		{
			generators_.insert({ command, generator });
		}

		int command_interface::run(int argc, char** argv)
		{
			if (!parse_command(argc, argv))
			{
				return 0;
			}

			auto path = std::filesystem::path(input_dir_);

			if (path.empty())
			{
				path = std::filesystem::current_path();
			}

			for (auto& file : input_files_)
			{
				auto target = path.append(file);

				auto iter = schemas_.find(target.extension().string());

				if (iter == schemas_.end())
				{
					std::cout << target.extension() << " is not supported!\n";
					continue;
				}

				auto datas = iter->second->run(target.string(), protocol_);

				auto it = generators_.find(output_state_);

				if (it == generators_.end())
				{
					std::cout << "generator[" << output_state_ << "] is not support\n";
					continue;
				}

				auto target_path = target.parent_path();
				target_path.append(target.filename().string());

				std::fstream header(target_path.string() + ".h", std::ios::out | std::ios::binary | std::ios::ate);
				std::fstream source(target_path.string() + ".cpp", std::ios::out | std::ios::binary | std::ios::ate);

				if (!header.is_open() || !source.is_open())
				{
					std::cout << "output file is not exist!\n";
					return false;
				}

				header << "#pragma once\n";
				header << "#include <aquarius.hpp>\n";

				source << "#include \"" << target_path.filename().string() << ".h\"\n";

				it->second->run(header, source, datas, protocol_);
			}

			return 0;
		}

		bool command_interface::parse_command(int argc, char** argv)
		{
			if (argc < 2)
			{
				help();
				return false;
			}

			std::string line{};

			auto make_error = [&]
			{
				std::cout << "Unkown Options:" << line << "\n";
				return false;
			};

			for (int i = 1; i < argc; ++i)
			{
				line = std::string(argv[i]);

				if (line.size() < 2)
				{
					return make_error();
				}

				if (line[0] != '-')
				{
					if (!check_suffix(line))
					{
						std::cout << "Input file error! there must be [.virgo] or [.tbl] file!\n";
						return false;
					}

					input_files_.push_back(line);

					continue;
				}

				if (line[1] == '-')
				{
					auto pos = line.find("=");
					if (pos == std::string::npos)
					{
						return make_error();
					}

					auto command = line.substr(0, pos);
					auto value = line.substr(pos + 1);

					if (!parse_line(command, value))
					{
						return false;
					}
				}
				else
				{
					auto pos = line.find_first_of(' ');
					if (pos == std::string::npos)
					{
						return make_error();
					}

					line = line.substr(1, pos - 1);
					if (line.size() != 1)
					{
						return make_error();
					}

					if (!parse_line(line, ""))
					{
						return false;
					}
				}
			}

			return true;
		}

		bool command_interface::parse_line(const std::string& command, const std::string& value)
		{
			if (command == "--help" || command == "-h")
			{
				help();
				return false;
			}
			else if (command == "--version" || command == "-v")
			{
				std::cout << get_version();
				return false;
			}
			else if (command == "--cpp_out")
			{
				if (!std::filesystem::is_directory(value))
				{
					std::cout << value << " That is not a correct file path\n";
					return false;
				}

				if (value.empty())
				{
					std::cout << "No output file path and that can not generate protocol files!\n";
					return 0;
				}

				output_file_path_ = std::filesystem::absolute(value);

				output_state_ = command;
			}
			else if (command == "--input_dir")
			{
				if (!std::filesystem::is_directory(value))
				{
					std::cout << value << " that is not a correct file path\n";
					return false;
				}

				input_dir_ = std::filesystem::absolute(value);
			}
			else if (command == "--protocol" || command == "-p")
			{
				protocol_ = value;
			}

			return true;
		}

		std::string command_interface::get_version()
		{
			return "lazytool version 0.1";
		}

		bool command_interface::check_suffix(const std::string& file_path)
		{
			std::filesystem::path file(file_path);

			return file.extension().compare(__virgo__) == 0;
		}

		void command_interface::help()
		{
			std::cout << "Usage:\n";
			std::cout << std::setw(3) << " " << std::left << std::setw(50)
					  << "use lazytool [<file>.virgo <file>.virgo ...] to generate protocol files. \n";
			std::cout << "Options:\n";
			std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--help -h"
					  << "Display this help message\n";
			std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--version -v"
					  << "Display version information\n";
			std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--protocol -p"
					  << "generate protocol file with protocol[tcp|http]\n";
			std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--input_dir=<file_dirs> "
					  << "input dirs to parse that will parse all files in directory\n";
			std::cout << std::setw(3) << " " << std::left << std::setw(50) << "--{language}_out=<file_path>"
					  << "[language:cpp] Generate [language] code and output on <file_path>\n";
		}
	} // namespace lazytool
} // namespace aquarius