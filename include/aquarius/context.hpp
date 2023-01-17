#pragma once
#include <aquarius/impl/context.hpp>
#include <aquarius/impl/router.hpp>

namespace aquarius
{
#define MESSAGE_DEFINE(msg) static aquarius::impl::msg_regist<msg> msg##msg(msg::Number);

#define CONTEXT_DEFINE(msg, context) \
	MESSAGE_DEFINE(msg) static aquarius::impl::ctx_regist<context> ctx##context(msg::Number);



} // namespace aquarius