#pragma once
#include <string>

namespace aquarius
{
	namespace gateway
	{
		struct account
		{
			int64_t id;
			std::string user;
			std::string passwd;
		};
	} // namespace gateway
} // namespace aquarius