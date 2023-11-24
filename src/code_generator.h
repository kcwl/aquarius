#pragma once
#include <string>

#include "file_descriptor.h"

namespace elastic
{
	namespace compiler
	{
		class code_generator
		{
		public:
			code_generator() = default;
			code_generator(const code_generator&) = delete;
			code_generator(code_generator&&) = delete;

			virtual ~code_generator() = default;

		public:
			virtual bool generate(file_descriptor* input_file, const std::string& output_dir) = 0;
		};
	} // namespace compiler
} // namespace elastic
