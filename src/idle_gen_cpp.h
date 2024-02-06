#pragma once
#include "code_generator.h"

#include <map>
#include <queue>
#include <vector>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace compiler
	{
		namespace cpp
		{
			class generate_cpp : public code_generator
			{
			public:
				generate_cpp() = default;

				virtual ~generate_cpp() = default;

			public:
				virtual bool generate(file_descriptor* input_file, const std::string& output_dir) override;

			private:
				void write_struct_declare();

				void write_struct_declare_header();

				void begin_write_package(const std::string& name);

				void end_write_package();

				void begin_write_class(const reflactor_structure& rs);

				void write_access_func(const reflactor_structure& rs);

				void write_members(const reflactor_structure& srs);

				void end_write_class();

				void real_write_file(std::ofstream& ofs);

				void pragma(const std::string& name);

				void include_file(const std::string& file_name, bool has_libary = true);

				void line_feed();

				void write_message_register(const std::vector<reflactor_structure>& rs);

			private:
				file_descriptor* input_file_ptr_;

				std::string output_file_name_;

				std::ofstream write_hpp_stream_;

				std::ofstream write_cpp_stream_;

				std::vector<std::string> lines;
			};
		} // namespace cpp
	}	  // namespace compiler
} // namespace elastic
