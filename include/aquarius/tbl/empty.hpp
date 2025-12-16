#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/tbl/database_param.hpp>

namespace aquarius
{
	namespace tbl
	{
		class empty_impl
		{
		public:
			empty_impl(const database_param&)
			{}
			virtual ~empty_impl() = default;

		public:
			auto async_connect() -> awaitable<error_code>
			{
				co_return error_code{};
			}

			auto async_execute(std::string_view sql, error_code& ec) -> awaitable<std::size_t>
			{
				co_return 0;
			}

			template <typename T>
			auto async_query(std::string_view sql, error_code& ec) -> awaitable<std::vector<T>>
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
