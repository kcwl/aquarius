#include "mysql_module.h"

namespace aquarius
{
	namespace db
	{
		mysql_module::mysql_module()
			: index_(0)
			, mutex_()
			, connector_(nullptr)
		{}

		auto mysql_module::run() -> asio::awaitable<bool>
		{
			mysql_config& cfg = create_mysql();

			mysql::pool_params params{};
			params.server_address.emplace_host_and_port(cfg.host, static_cast<uint16_t>(cfg.port));
			params.username = cfg.user;
			params.password = cfg.password;
			params.database = cfg.db;
			params.ssl = mysql::ssl_mode::disable;

			connector_ = std::make_shared<sql_op_t>(std::move(params));

			co_await connector_->async_run();

			co_return true;
		}

		auto mysql_module::async_query_only(std::string_view sql) -> asio::awaitable<std::vector<std::string>>
		{
			error_code ec{};

			auto results = co_await connector_->async_query_only(sql, ec);

			if (ec)
			{
				XLOG_ERROR() << "[async query] error: " << ec.what() << ", sql:" << sql;
			}

			co_return results;
		}

		auto mysql_module::async_execute(std::string_view sql) -> asio::awaitable<std::size_t>
		{
			error_code ec{};

			auto results = co_await connector_->async_execute(sql, ec);

			if (ec)
			{
				XLOG_ERROR() << "[async query] error: " << ec.what() << ", sql:" << sql;
			}

			co_return results;
		}

		auto mysql_module::async_multi_execute(const std::vector<std::string>& sqls) -> asio::awaitable<std::size_t>
		{
			error_code ec{};

			co_await connector_->begin();

			for (auto& s : sqls)
			{
				auto result = co_await async_execute(s);

				if (result == 0)
				{
					connector_->rollback();
					co_return result;
				}
			}

			connector_->commit();

			co_return sqls.size();
		}
	}
}