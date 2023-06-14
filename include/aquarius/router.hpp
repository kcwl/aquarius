#pragma once
#include <aquarius/detail/router.hpp>
#include <aquarius/message.hpp>

namespace aquarius
{
	namespace detail
	{
		class context;

		using ctx_router = detail::single_router<std::shared_ptr<context>>;

		using msg_router = detail::single_router<std::shared_ptr<xmessage>>;
	} // namespace detail
} // namespace aquarius