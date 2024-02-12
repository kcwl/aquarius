#pragma once
#include <aquarius/detail/router.hpp>

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class msg_router : public detail::single_router<msg_router, std::shared_ptr<basic_message>>
	{};
} // namespace aquarius