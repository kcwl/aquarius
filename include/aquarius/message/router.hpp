#pragma once
#include <aquarius/detail/router.hpp>

namespace aquarius
{
	class context;

	class xmessage;
} // namespace aquarius

namespace aquarius
{
	class ctx_router : public detail::single_router<ctx_router, std::shared_ptr<context>>
	{};

	class msg_router : public detail::single_router<msg_router, std::shared_ptr<xmessage>>
	{};
} // namespace aquarius