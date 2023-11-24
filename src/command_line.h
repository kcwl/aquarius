#pragma once
#include "code_generator.h"

#include <map>

namespace elastic
{
	namespace compiler
	{
		struct generator_info
		{
			std::string flag_name_;
			std::string option_flag_name_;
			code_generator* generator_;
			std::string help_text_;
		};

		class command_line
		{
		public:
			command_line() = default;

			command_line(const command_line&) = delete;

			command_line& operator=(const command_line&) = delete;

		public:
			void register_generator(const std::string& flag_name, code_generator* generator,
									const std::string& help_text);

			void register_generator(const std::string& flag_name, const std::string& option_flag_name,
									code_generator* generator, const std::string& help_text);

			int run(int argc, char** argv);

		private:
			void parse_argument(int argc, char** argv);

		private:
			std::map<std::string, generator_info> generators_;

			std::string executable_name_;

			std::string compile_flag_;

			std::string input_file_;

			std::string output_dir_;
		};
	} // namespace compiler
} // namespace elastic
