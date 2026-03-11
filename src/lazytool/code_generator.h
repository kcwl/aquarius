#pragma once
#include "field.h"
#include <fstream>
#include <vector>

namespace aquarius
{
	namespace lazytool
	{
		class code_generator
		{
		public:
			virtual bool run(std::fstream&, std::fstream&, const std::vector<std::shared_ptr<field>>&) = 0;
		};
	} // namespace lazytool
} // namespace aquarius