#pragma once
#include <aquarius/impl/router.hpp>
#include <aquarius/impl/context.hpp>

namespace aquarius
{
#define CONTEXT_DEFINE(msg, context) static aquarius::impl::ctx_regist<context> ctx##context(msg::Number);

} // namespace aquarius