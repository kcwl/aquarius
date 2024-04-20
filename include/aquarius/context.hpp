#pragma once
#include <aquarius/context/content.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/core/elastic.hpp>
#include <aquarius/message/generator.hpp>

#define AQUARIUS_MESSAGE_REGIST(req) static aquarius::msg_regist<req> msg_##req(req::Number)

#define MESSAGE_MULTI_DEFINE(name_space, req)                                                                          \
	static aquarius::msg_regist<name_space::req> msg_##req(name_space::req::Number)

#define CONTEXT_DEFINE_IMPL(msg, basic_context)                                                                        \
	static aquarius::ctx_regist<basic_context> ctx_##basic_context(msg::Number)

#define CONTEXT_IMPL_MULTI_DEFINE(msg, name_space, basic_context)                                                      \
	static aquarius::ctx_regist<name_space::basic_context> ctx##basic_context(msg::Number)

#define AQUARIUS_CONTEXT_REGIST(msg, basic_context)                                                                    \
	AQUARIUS_MESSAGE_REGIST(msg);                                                                                      \
	CONTEXT_DEFINE_IMPL(msg, basic_context)

#define CONTEXT_DEFINE_MULTI_MESSAGE(name_space, req, basic_context)                                                   \
	MESSAGE_MULTI_DEFINE(name_space, req);                                                                             \
	CONTEXT_DEFINE_IMPL(name_space::req, basic_context)

#define CONTEXT_DEFINE_MULTI_CONTEXT (msg, name_space, basic_context)

#define CONTEXT_MULTI_DEFINE(msg_name_space, msg, ctx_name_space, basic_context)                                       \
	MESSAGE_MULTI_DEFINE(msg_name_space, msg);                                                                         \
	static aquarius::ctx_regist<ctx_name_space::basic_context> ctx_##basic_context(msg_name_space::msg::Number)

#define CONTEXT_SYSTEM_DEFINE(basic_context)                                                                           \
	static aquarius::ctx_regist<basic_context> ctx_system_##basic_context(basic_context::Number)

#define AQUARIUS_ACCESS(...) ELASTIC_ACCESS(__VA_ARGS__)

#define AQUARIUS_ACCESS_IF(base, ...) ELASTIC_ACCESS_IF(base, __VA_ARGS__)

#define AQUARIUS_ACCESS_NULL()                                                                                         \
private:                                                                                                               \
	friend class elastic::access;                                                                                      \
	template <typename _Archive>                                                                                       \
	void serialize(_Archive& ar)                                                                                       \
	{}