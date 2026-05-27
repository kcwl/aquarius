#pragma once
#include "gate_error_code.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace gateway
	{
		template <typename Func, typename Protocol, typename... Args>
		class basic_transfer_context : public basic_protocol_context<Protocol, Args...>
		{
		public:
			using base_type = basic_protocol_context<Protocol, Args...>;

			using transfer_func_t = std::function<asio::awaitable<error_code>(std::size_t,flex_buffer&)>;

		public:
			basic_transfer_context(Func&& func)
				: base_type(&basic_transfer_context<Func, Protocol, Args...>::do_complete)
				, func_(func)
			{}

			virtual ~basic_transfer_context() = default;

		public:
			static auto do_complete(base_type* ctx, Protocol* proto, std::size_t session_id, flex_buffer& buffer, Args&&... args)
				-> asio::awaitable<error_code>
			{
				auto context = static_cast<basic_transfer_context*>(ctx);

				if (!context)
				{
					co_return gate_op::not_exist_in_pool;
				}

				co_return co_await context->func_(session_id, buffer, std::forward<Args>(args)...);
			}

		private:
			transfer_func_t func_;
		};

		template <typename Func, typename Protocol, typename... Args>
		basic_transfer_context(Func) -> basic_transfer_context<Protocol, Args...>;
	} // namespace gateway
} // namespace aquarius