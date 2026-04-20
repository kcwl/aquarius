#pragma once
#include <aquarius/basic_module.hpp>
#include <aquarius/module/module_register.hpp>
#include <aquarius/resource/mysql_config.hpp>
#include <aquarius/tbl/mysql_io_service.hpp>

namespace aquarius
{
	AQUARIUS_MODULE(mysql_module)
	{
		using sql_op_t = mysql_io_service;

	public:
		mysql_module()
			: index_(0)
			, mutex_()
			, connector_(nullptr)
		{}

		virtual ~mysql_module() = default;

	public:
		virtual bool init() override
		{
			return true;
		}

		virtual auto run() -> asio::awaitable<bool> override
		{
			mysql_config cfg{};

			cfg_value_from<mysql_config>(cfg);

			boost::mysql::pool_params params{};
			params.server_address.emplace_host_and_port(cfg.host, static_cast<uint16_t>(cfg.port));
			params.username = cfg.user;
			params.password = cfg.password;
			params.database = cfg.db;
			params.ssl = boost::mysql::ssl_mode::disable;

			auto executor = co_await asio::this_coro::executor;

			connector_ = std::make_shared<sql_op_t>(executor, std::move(params));

			connector_->async_run();

			co_return true;
		}

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

		auto async_execute(std::string_view sql) -> asio::awaitable<std::size_t>
		{
			error_code ec{};

			auto results = co_await connector_->async_execute(sql, ec);

			if (ec)
			{
				XLOG_ERROR() << "[async query] error: " << ec.what() << ", sql:" << sql;
			}

			co_return results;
		}

		auto async_multi_execute(const std::vector<std::string>& sqls) -> asio::awaitable<std::size_t>
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

	private:
		std::size_t index_;

		std::mutex mutex_;

		std::shared_ptr<sql_op_t> connector_;
	};
} // namespace aquarius
