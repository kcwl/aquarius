#pragma once
#include "code_generator.h"
#include "schema_parser.h"
#include <filesystem>
#include <map>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace lazytool
	{
		class command_interface final
		{
			constexpr static auto __virgo__ = ".virgo"sv;

		public:
			command_interface();
			~command_interface() = default;

		public:
			void regist(const std::string& command, code_generator* generator);

			int run(int argc, char** argv);

			void help();

		private:
			bool parse_command(int argc, char** argv);

			bool parse_line(const std::string& command, const std::string& value);

			std::string get_version();

			bool check_suffix(const std::string& file_path);

		private:
			std::map<std::string, code_generator*> generators_;

			std::map<std::string_view, std::shared_ptr<schema_parser>> schemas_;

			std::vector<std::string> input_files_;

			std::filesystem::path output_file_path_;

			std::filesystem::path input_dir_;

			std::string protocol_;

			std::string output_state_;
		};
	} // namespace lazytool
} // namespace aquarius