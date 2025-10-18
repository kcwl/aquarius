#pragma once
#include "parser.h"

namespace aquarius
{
	namespace lazytool
	{
		class parser;

		class generator
		{
		public:
			enum class generate_error
			{
				success,
				empty
			};

		public:
			generator() = default;
			virtual ~generator() = default;

		public:
			virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
										 std::ofstream& ofs_cpp) = 0;
		};
	} // namespace lazytool
} // namespace aquarius