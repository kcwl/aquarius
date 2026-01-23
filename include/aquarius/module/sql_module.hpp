#pragma once
#include <aquarius/io_service_pool.hpp>
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
			, pool_ptr_(nullptr)
			, index_(0)
			, mutex_()
			, connectors_()
		{}

	public:
		virtual bool init() override
		{
			pool_ptr_= std::make_shared<io_service_pool>(configs().pool_size);

			

			for (int i = 0; i < pool_ptr_->pool_size(); ++i)
			{
				boost::mysql::pool_params params{};
				params.server_address.emplace_host_and_port(configs().host, static_cast<uint16_t>(configs().port));
				params.username = configs().user;
				params.password = configs().password;
				params.database = configs().db;

				auto connector_ptr = std::make_shared<service>(pool_ptr_->get_io_service(), std::move(params));
				connectors_.push_back(connector_ptr);
			}

			return true;
		}

		virtual auto run(io_context&) -> awaitable<void> override
		{
			for (auto& connector : connectors_)
			{
				if (!connector)
					continue;

				connector->async_run();
			}

			co_return;
		}

		template <typename T>
		auto async_query(std::string_view sql) -> awaitable<std::vector<T>>
		{
			auto connector = get_connector();

			co_return co_await connector->async_query<T>(sql);
		}

		auto async_execute(std::string_view sql) -> awaitable<std::size_t>
		{
			auto connector = get_connector();

			error_code ec{};

			co_return co_await connector->async_execute(sql, ec);
		}

	private:
		std::shared_ptr<service> get_connector()
		{
			std::lock_guard lock(mutex_);

			if (connectors_.empty())
			{
				XLOG_WARNING() << "sql pool is empty!";

				return nullptr;
			}

			if (index_ == connectors_.size())
				index_ = 0;

			return connectors_[index_++];
		}

	private:
		std::shared_ptr<io_service_pool> pool_ptr_;

		std::size_t index_;

		std::mutex mutex_;

		std::vector<std::shared_ptr<service>> connectors_;
	};
} // namespace aquarius