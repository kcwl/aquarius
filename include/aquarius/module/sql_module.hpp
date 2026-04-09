#pragma once
#include <aquarius/module/module.hpp>
#include <aquarius/module/schedule.hpp>
#include <aquarius/resource/global_resource.hpp>
#include <aquarius/tbl/sql_op.hpp>

namespace aquarius
{
	template <typename Executor = asio::any_io_executor>
	class sql_module : public _module<sql_module<Executor>>
	{
		using executor_type = Executor;

		using sql_op_t = sql_op<executor_type>;

	public:
		sql_module(const std::string& name)
			: _module<sql_module>(name)
			, index_(0)
			, mutex_()
			, connector_(nullptr)
		{}

		virtual ~sql_module() = default;

	public:
		virtual bool init() override
		{
			return true;
		}

		virtual auto run() -> asio::awaitable<void> override
		{
			auto& config = global_resource::get_mutable_instance().mysql();

			boost::mysql::pool_params params{};
			params.server_address.emplace_host_and_port(config.host, static_cast<uint16_t>(config.port));
			params.username = config.user;
			params.password = config.password;
			params.database = config.db;
			params.ssl = boost::mysql::ssl_mode::disable;

			auto executor = co_await asio::this_coro::executor;

			connector_ = std::make_shared<sql_op_t>(executor);

			connector_->async_run();

			co_return;
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

		auto async_execute(const std::vector<std::string>& sqls) -> asio::awaitable<std::size_t>
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

	template <typename T>
	inline auto mpc_query(std::string_view sql) -> asio::awaitable<std::vector<T>>
	{
		co_return co_await mpc::call<std::vector<T>, sql_module<>>(
			[&](sql_module<>* ptr) -> asio::awaitable<std::vector<T>>
			{ co_return co_await ptr->template async_query<T>(sql); });
	}

	inline auto mpc_execute(std::string_view sql) -> asio::awaitable<std::size_t>
	{
		co_return co_await mpc::call<std::size_t, sql_module<>>(
			[&](sql_module<>* ptr) -> asio::awaitable<std::size_t>
			{ co_return co_await ptr->async_execute(sql); });
	}

	inline auto mpc_execute(const std::vector<std::string>& sqls) -> asio::awaitable<bool>
	{
		co_return co_await mpc::call<std::size_t, sql_module<>>(
			[&](sql_module<>* ptr) -> asio::awaitable<bool> { co_return co_await ptr->async_execute(sqls); });
	}

	inline auto mpc_insert(std::string_view sql) -> asio::awaitable<std::size_t>
	{
		co_return co_await mpc_execute(sql);
	}

	inline auto mpc_delete(std::string_view sql) -> asio::awaitable<std::size_t>
	{
		co_return co_await mpc_execute(sql);
	}

	inline auto mpc_update(std::string_view sql) -> asio::awaitable<std::size_t>
	{
		co_return co_await mpc_execute(sql);
	}

	template<typename T>
	inline auto mpc_select(std::string_view sql) -> asio::awaitable<std::vector<T>>
	{
		co_return co_await mpc_query<T>(sql);
	}
} // namespace aquarius
