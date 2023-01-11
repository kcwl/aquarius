#pragma once
#include <aquarius/impl/router.hpp>

namespace aquarius
{
#define CONTEXT_DEFINE(msg, context) static aquarius::impl::ctx_regist<msg, context> ctx##context(msg::Number);

} // namespace aquarius