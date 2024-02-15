#pragma once
#include <aquarius/detail/router.hpp>

namespace aquarius
{
	class basic_context;
} // namespace aquarius

namespace aquarius
{
	class router_context : public detail::single_router<router_context, std::shared_ptr<basic_context>>
	{};
} // namespace aquarius