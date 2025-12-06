#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/sql/database_param.hpp>

namespace aquarius
{
	namespace sql
	{
		template <typename Executor>
		class empty_impl
		{
		public:
			empty_impl(const Executor&, const database_param&)
			{}
			virtual ~empty_impl() = default;

		public:
			template <typename CompleteToken = default_completion_token_t<Executor>>
			auto async_connect(CompleteToken&& token = default_completion_token_t<Executor>()) -> awaitable<void>
			{
				co_return;
			}

			template <typename CompleteToken = default_completion_token_t<Executor>>
			auto async_execute(std::string_view sql, CompleteToken&& token = default_completion_token_t<Executor>())
				-> awaitable<std::size_t>
			{
				co_return 0;
			}

			template <typename T, typename CompleteToken = default_completion_token_t<Executor>>
			auto async_query(std::string_view sql, CompleteToken&& token = default_completion_token_t<Executor>())
				-> awaitable<std::vector<T>>
			{
				co_return std::vector<T>{};
			}

			void enable_transaction()
			{
				return;
			}

			void disable_transaction()
			{
				return;
			}

			error_code begin()
			{
				return error_code{};
			}

			error_code commit()
			{
				return error_code{};
			}

			error_code rollback()
			{
				return error_code{};
			}
		};

	} // namespace sql
} // namespace aquarius
