#pragma once
#include <aquarius/context/system_context.hpp>
#include <aquarius/context/user_context.hpp>

#define AQUARIUS_MESSAGE_REGIST(req) static aquarius::msg_regist<req> msg_##req(req::Number)

#define MESSAGE_MULTI_DEFINE(name_space, req)                                                                          \
	static aquarius::ctx::msg_regist<name_space::req> msg_##req(name_space::req::Number)

#define CONTEXT_DEFINE_IMPL(msg, context) static aquarius::ctx_regist<context> ctx_##context(msg::Number)

#define CONTEXT_IMPL_MULTI_DEFINE(msg, name_space, context)                                                            \
	static aquarius::ctx_regist<name_space::context> ctx##context(msg::Number)

#define AQUARIUS_CONTEXT_REGIST(msg, context)                                                                                   \
	AQUARIUS_MESSAGE_REGIST(msg);                                                                                               \
	CONTEXT_DEFINE_IMPL(msg, context)

#define CONTEXT_DEFINE_MULTI_MESSAGE(name_space, req, context)                                                         \
	MESSAGE_MULTI_DEFINE(name_space, req);                                                                             \
	CONTEXT_DEFINE_IMPL(name_space::req, context)

#define CONTEXT_DEFINE_MULTI_CONTEXT (msg, name_space, context)

#define CONTEXT_MULTI_DEFINE(msg_name_space, msg, ctx_name_space, context)                                             \
	MESSAGE_MULTI_DEFINE(msg_name_space, msg);                                                                         \
	static aquarius::ctx_regist<ctx_name_space::context> ctx_##context(msg_name_space::msg::Number)

#define CONTEXT_SYSTEM_DEFINE(context) static aquarius::ctx_regist<context> ctx_system_##context(context::Number)