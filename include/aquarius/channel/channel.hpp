#pragma once
#include <aquarius/channel/impl/channel.hpp>
#include <aquarius/channel/impl/role.hpp>
#include <memory>
#include <string>
#include <vector>

namespace aquarius
{
	namespace channel
	{
		using default_channel = basic_channel<impl::role>;
	} // namespace channel
} // namespace aquarius