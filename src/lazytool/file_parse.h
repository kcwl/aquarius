#pragma once
#include "registor.h"
#include <memory>
#include <vector>

namespace aquarius
{
	namespace lazytool
	{
		class parser;

		class file_parser
		{
		public:
			file_parser();
			~file_parser() = default;

		public:
			bool read_file(const std::string& file_path);

		public:
			std::vector<std::shared_ptr<parser>> keywords_;

			registor registor_;

			std::size_t column_;

			std::size_t row_;
		};
	} // namespace lazytool
} // namespace aquarius
