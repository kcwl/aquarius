#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "file_struct.hpp"

namespace aquarius
{
	namespace compiler
	{
		class file_descriptor
		{
		public:
			file_descriptor()
				: read_file_stream_()
				, input_file_name_()
				, multi_key_words_()
			{}

		public:
			bool read_file(const std::string& file_name);

			std::string file_name() const;

			std::vector<reflactor_structure> structs() const;

		private:
			bool read_to_spilt(std::string& value, int sp);

			bool read_command(std::string& name);

			bool check_file_suffix(const std::string& file_name);

			bool read_keyword(std::string& keyword);

			bool read_keytype(std::string& keytype);

			bool read_message(reflactor_structure& rs);

			bool read_package(reflactor_structure& rs);

			std::string spilt_keyword_from_line(const std::string& keyword);

			std::string spilt_keytype_from_line(const std::string& keytype);

			bool check_space(const std::string& word);

		private:
			std::ifstream read_file_stream_;

			std::string input_file_name_;

			std::vector<reflactor_structure> multi_key_words_;
		};
	} // namespace compiler
} // namespace elastic