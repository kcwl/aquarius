#pragma once
#include <aquarius/detail/context.hpp>
#include <aquarius/detail/invoke.hpp>
#include <aquarius/router.hpp>

#define MESSAGE_DEFINE(req) static aquarius::detail::msg_regist<req> msg_##req(req::Number)

#define MESSAGE_MULTI_DEFINE(name_space, req)                                                                          \
	static aquarius::ctx::msg_regist<name_space::req> msg_##req(name_space::req::Number)

#define CONTEXT_DEFINE_IMPL(msg, context) static aquarius::detail::ctx_regist<context> ctx_##context(msg::Number)

#define CONTEXT_IMPL_MULTI_DEFINE(msg, name_space, context)                                                            \
	static aquarius::detail::ctx_regist<name_space::context> ctx##context(msg::Number)

#define CONTEXT_DEFINE(msg, context)                                                                                   \
	MESSAGE_DEFINE(msg);                                                                                               \
	CONTEXT_DEFINE_IMPL(msg, context)

#define CONTEXT_DEFINE_MULTI_MESSAGE(name_space, req, context)                                                         \
	MESSAGE_MULTI_DEFINE(name_space, req);                                                                             \
	CONTEXT_DEFINE_IMPL(name_space::req, context)

#define CONTEXT_DEFINE_MULTI_CONTEXT (msg, name_space, context)

#define CONTEXT_MULTI_DEFINE(msg_name_space, msg, ctx_name_space, context)                                             \
	MESSAGE_MULTI_DEFINE(msg_name_space, msg);                                                                         \
	static aquarius::detail::ctx_regist<ctx_name_space::context> ctx_##context(msg_name_space::msg::Number)

namespace aquarius
{
	template <typename _Request, typename _Response>
	class context : public detail::basic_context, public detail::visitor<_Request, int>
	{
	public:
		context(const std::string& name)
			: basic_context(name)
			, request_ptr_(nullptr)
		{}

	public:
		virtual int visit(_Request* req, std::shared_ptr<detail::transfer> conn_ptr)
		{
			request_ptr_ = req;

			conn_ptr_ = conn_ptr;

			return handle();
		}

	protected:
		virtual int handle() = 0;

		bool send_response(int result)
		{
			response_.header().clone(request_ptr_->header());

			response_.header().result_ = result;

			flex_buffer_t fs;

			response_.visit(fs, visit_mode::output);

			(*conn_ptr_)(std::move(fs));

			return true;
		}

	protected:
		_Request* request_ptr_;

		_Response response_;
	};
} // namespace aquarius