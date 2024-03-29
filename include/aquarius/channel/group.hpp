#pragma once
#include <aquarius/channel/channel.hpp>
#include <aquarius/channel/impl/group.hpp>
#include <aquarius/core/singleton.hpp>
#include <shared_mutex>
#include <unordered_map>

namespace aquarius
{
	namespace channel
	{
		using default_group = impl::basic_group<default_channel>;
	}
} // namespace aquarius