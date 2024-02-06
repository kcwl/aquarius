#include "command_line.h"

#include "file_descriptor.h"

#include <vector>

namespace aquarius
{
	namespace compiler
	{
		void command_line::register_generator(const std::string& flag_name, code_generator* generator,
											  const std::string& help_text = {})
		{
			generator_info info{};
			info.flag_name_ = flag_name;
			info.generator_ = generator;
			info.help_text_ = help_text;

			generators_[flag_name] = info;
		}

		void command_line::register_generator(const std::string& flag_name, const std::string& option_flag_name,
											  code_generator* generator, const std::string& help_text = {})
		{
			generator_info info{};
			info.flag_name_ = flag_name;
			info.option_flag_name_ = option_flag_name;
			info.generator_ = generator;
			info.help_text_ = help_text;

			generators_[flag_name] = info;
		}

		int command_line::run(int argc, char** argv)
		{
			parse_argument(argc, argv);

			file_descriptor file{};

			if (!input_file_.empty())
			{
				file.read_file(input_file_);
			}

			auto iter = generators_.find(compile_flag_);
			if (iter == generators_.end())
				return 0;

			iter->second.generator_->generate(&file, output_dir_);

			return 1;
		}

		void command_line::parse_argument(int argc, char** argv)
		{
			executable_name_ = argv[0];

			std::vector<std::string> arguments;
			for (int i = 1; i < argc; ++i)
			{
				arguments.push_back(argv[i]);
			}

			// if no arguments are given, show help
			if (arguments.empty())
			{
				return;
			}

			for (auto& ar : arguments)
			{
				auto pos = ar.find_first_of("=");

				if (pos == std::string::npos)
					continue;

				auto name = ar.substr(0, pos);
				auto value = ar.substr(pos + 1);

				if (name.find("_out") != std::string::npos)
				{
					compile_flag_ = name;
					output_dir_ = value;
				}

				if (name.find("--input") != std::string::npos)
				{
					input_file_ = value;
				}
			}
		}
	} // namespace compiler
} // namespace elastic