#pragma once
#include <aquarius/impl/router.hpp>

namespace aquarius
{
#define CONTEXT_DEFINE_SERVER(req, context) static aquarius::impl::ctx_regist<req, context> ctx##context(req::Number);
#define CONTEXT_DEFINE_CLIENT(resp, context)                                                                           \
	static aquarius::impl::ctx_regist<resp, context> ctx##context(resp::Number);

} // namespace aquarius