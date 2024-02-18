#pragma once
#include <aquarius/system/router.hpp>

namespace aquarius
{
	class basic_context;
} // namespace aquarius

namespace aquarius
{
	class router_context : public system::single_router<router_context, std::shared_ptr<basic_context>>
	{};
} // namespace aquarius