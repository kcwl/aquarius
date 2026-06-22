#pragma once
#include <aquarius/singleton.hpp>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		struct global_config : singleton<global_config>
		{
			std::string proto;
		};
	}
}