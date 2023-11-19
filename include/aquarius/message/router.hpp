#pragma once
#include <aquarius/detail/router.hpp>

namespace aquarius
{
	class context;

	class xmessage;
}

namespace aquarius
{
	using ctx_router = detail::single_router<std::shared_ptr<context>>;

	using msg_router = detail::single_router<std::shared_ptr<xmessage>>;
} // namespace aquarius