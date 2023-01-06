#pragma once
#include <aquarius/impl/router.hpp>

namespace aquarius
{
#define MESSAGE_DEFINE(type) static aquarius::impl::msg_regist<type> req##type(type::Number);
#define CONTEXT_DEFINE(type, number) static aquarius::impl::ctx_regist<type> ctx##type(number);

} // namespace aquarius