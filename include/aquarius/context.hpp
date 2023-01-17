#pragma once
#include <aquarius/impl/context.hpp>
#include <aquarius/impl/router.hpp>

namespace aquarius
{
#define CONTEXT_DEFINE(msg, context) static aquarius::impl::ctx_regist<context> ctx##context(msg::Number);

#define MESSAGE_DEFINE(msg) static aquarius::impl::msg_regist<msg> msg##msg(msg::Number);

} // namespace aquarius