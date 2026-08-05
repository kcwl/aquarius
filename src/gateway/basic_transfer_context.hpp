#pragma once
#include "gate_error_code.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace gateway
	{
		template <typename Protocol, typename... Args>
		class basic_transfer_context : public basic_protocol_context<Protocol, Args...>
		{
		public:
			using base_type = basic_protocol_context<Protocol, Args...>;

			using session_callback = typename base_type::session_callback;

			using transfer_func_t =
				std::function<asio::awaitable<error_code>(flex_buffer&, const std::string&, const session_callback&)>;

		public:
			basic_transfer_context(const transfer_func_t& func)
				: base_type(&basic_transfer_context<Protocol, Args...>::do_complete)
				, func_(func)
			{}

			virtual ~basic_transfer_context() = default;

		public:
			static auto do_complete(base_type* ctx, Protocol* proto, flex_buffer& buffer, const session_callback& cb)
				-> asio::awaitable<error_code>
			{
				auto context = static_cast<basic_transfer_context*>(ctx);

				if (!context)
				{
					co_return gate_op::not_exist_in_pool;
				}

				co_return co_await context->func_(buffer, context->router(), cb);
			}

		private:
			transfer_func_t func_;
		};

		template <typename Func, typename Protocol, typename... Args>
		basic_transfer_context(Func) -> basic_transfer_context<Func, Protocol, Args...>;
	} // namespace gateway
} // namespace aquarius