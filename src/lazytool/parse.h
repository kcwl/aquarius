#pragma once
#include "registor.h"
#include "keyword.h"

namespace aquarius
{
	namespace virgo
	{
		class parse
		{
		public:
			parse();

		public:
			bool read_file(const std::string& file_path, const std::string& output);

		private:
			std::vector<std::shared_ptr<proto_keyword>> pros_;

			registor regist_;

			std::size_t column_;

			std::size_t row_;
		};
	} // namespace virgo
} // namespace aquarius
