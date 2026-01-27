#pragma once
#include <aquarius/module/module.hpp>
#include <aquarius/tbl/engine/mysql.hpp>

namespace aquarius
{
	class sql_module : public _module<sql_module, database_param>
	{
		using service = tbl::mysql;

	public:
		sql_module(const std::string& name)
			: _module<sql_module, database_param>(name)
			, index_(0)
			, mutex_()
			, connector_(nullptr)
		{}

	public:
		virtual bool init() override
		{
			return true;
		}

		virtual auto run(io_context& io) -> awaitable<void> override
		{
			boost::mysql::pool_params params{};
			params.server_address.emplace_host_and_port(configs().host, static_cast<uint16_t>(configs().port));
			params.username = configs().user;
			params.password = configs().password;
			params.database = configs().db;
			params.ssl = boost::mysql::ssl_mode::disable;

			connector_ = std::make_shared<service>(io, std::move(params));

			co_await connector_->async_run();
		}

		template <typename T>
		auto async_query(std::string_view sql) -> awaitable<std::vector<T>>
		{
			error_code ec{};

			co_return co_await connector_->async_query<T>(sql, ec);
		}

		auto async_execute(std::string_view sql) -> awaitable<std::size_t>
		{
			error_code ec{};

			co_return co_await connector_->async_execute(sql, ec);
		}

		auto async_execute(const std::vector<std::string>& sqls) -> awaitable<std::size_t>
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

		std::shared_ptr<service> connector_;
	};
} // namespace aquarius
