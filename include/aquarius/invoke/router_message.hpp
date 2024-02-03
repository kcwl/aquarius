#pragma once
#include <aquarius/detail/router.hpp>

namespace aquarius
{
	class xmessage;
} // namespace aquarius

namespace aquarius
{
	class msg_router : public detail::single_router<msg_router, std::shared_ptr<xmessage>>
	{};
} // namespace aquarius