#pragma once
#include <aquarius/detail/router.hpp>
#include <aquarius/impl/message.hpp>

namespace aquarius
{
	namespace impl
	{
		using ctx_router = detail::single_router<std::shared_ptr<context>>;

		using msg_router = detail::single_router<std::shared_ptr<xmessage>>;
	} // namespace impl
} // namespace aquarius