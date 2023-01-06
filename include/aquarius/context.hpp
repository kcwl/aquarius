#pragma once
#include <aquarius/impl/router.hpp>

namespace aquarius
{
#define CONTEXT_DEFINE(req, context) static aquarius::impl::ctx_regist<req, context> ctx##context(##req);

} // namespace aquarius