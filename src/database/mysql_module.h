#pragma once
#include "mysql_config.hpp"
#include <aquarius/basic_module.hpp>
#include <aquarius/module/module_register.hpp>
#include <aquarius/tbl/mysql_io_service.hpp>

namespace aquarius
{
	namespace db
	{
		AQUARIUS_MODULE(mysql_module)
		{
			using sql_op_t = mysql_io_service;

		public:
			mysql_module();

			virtual ~mysql_module() = default;

		public:
			virtual auto run() -> asio::awaitable<bool> override;

			template <typename T>
			auto async_query(std::string_view sql) -> asio::awaitable<std::vector<T>>
			{
				error_code ec{};

				auto results = co_await connector_->template async_query<T>(sql, ec);

				if (ec)
				{
					XLOG_ERROR() << "[async query] error: " << ec.what() << ", sql:" << sql;
				}

				co_return results;
			}

			auto async_query_only(std::string_view sql) -> asio::awaitable<std::vector<std::string>>;

			auto async_execute(std::string_view sql) -> asio::awaitable<std::size_t>;

			auto async_multi_execute(const std::vector<std::string>& sqls) -> asio::awaitable<std::size_t>;

		private:
			std::size_t index_;

			std::mutex mutex_;

			std::shared_ptr<sql_op_t> connector_;
		};
	} // namespace db
} // namespace aquarius