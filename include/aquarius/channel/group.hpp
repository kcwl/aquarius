#pragma once
#include <aquarius/channel/impl/group.hpp>
#include <aquarius/core/singleton.hpp>
#include <unordered_map>
#include <shared_mutex>

namespace aquarius
{
	namespace channel
	{
		using default_group = impl::basic_group<default_channel>;
	}
}