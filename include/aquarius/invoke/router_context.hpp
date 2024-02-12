#pragma once
#include <aquarius/detail/router.hpp>

namespace aquarius
{
	class basic_context;
} // namespace aquarius

namespace aquarius
{
	class ctx_router : public detail::single_router<ctx_router, std::shared_ptr<basic_context>>
	{};
} // namespace aquarius