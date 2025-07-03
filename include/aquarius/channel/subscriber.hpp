#pragma once
#include <aquarius/channel/detail/basic_subscriber.hpp>

namespace aquarius
{
	namespace channel
	{
		using subscriber = detail::basic_subscriber<std::size_t>;
	}
}