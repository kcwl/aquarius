#pragma once
#include <aquarius/core/router.hpp>
#include <aquarius/tcp/message.hpp>

namespace aquarius
{
	namespace ctx
	{
		class context;

		using ctx_router = core::single_router<std::shared_ptr<context>>;

		using msg_router = core::single_router<std::shared_ptr<tcp::xmessage>>;
	} // namespace impl
} // namespace aquarius