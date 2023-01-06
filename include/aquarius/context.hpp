#pragma once
#include <aquarius/impl/router.hpp>

namespace aquarius
{
#define MESSAGE_DEFINE(base_type, type) static msg_regist < base_type, type> req##type(type::Number);
#define CONTEXT_DEFINE(type) static ctx_regist<type> ctx##type(type::Number);

}