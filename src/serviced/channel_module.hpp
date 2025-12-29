#pragma once
#include <aquarius/module/module.hpp>


namespace aquarius
{
	class channel_module : public no_config_module<channel<int>>
	{
	public:

	private:
		channel_group<channel<int>> group;
	};
}