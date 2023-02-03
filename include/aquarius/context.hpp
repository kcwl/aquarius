#pragma once
#include <aquarius/context/router.hpp>
#include <aquarius/context/context.hpp>

namespace aquarius
{
#define MESSAGE_DEFINE(msg) static aquarius::ctx::msg_regist<msg> msg##msg(msg::Number);

#define CONTEXT_DEFINE(msg, context)                                                                                   \
	MESSAGE_DEFINE(msg) static aquarius::ctx::ctx_regist<context> ctx##context(msg::Number);

} // namespace aquarius