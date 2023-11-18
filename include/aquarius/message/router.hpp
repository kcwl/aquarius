#pragma once
#include <aquarius/detail/router.hpp>
#include <aquarius/message/message.hpp>

namespace aquarius
{
	namespace detail
	{
		class basic_context;

		using ctx_router = detail::single_router<std::shared_ptr<basic_context>>;

		using msg_router = detail::single_router<std::shared_ptr<xmessage>>;
	} // namespace detail
} // namespace aquarius