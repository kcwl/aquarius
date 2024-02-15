#pragma once
#include <aquarius/detail/router.hpp>

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class router_message : public detail::single_router<router_message, std::shared_ptr<basic_message>>
	{};
} // namespace aquarius