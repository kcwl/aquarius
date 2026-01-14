#pragma once
#include <aquarius/module/module.hpp>
#include <aquarius/tbl/sql_connector.hpp>

namespace aquarius
{
	class sql_module : public _module<sql_module, database_param>
	{
	public:
		sql_module(const std::string& name)
			: _module<sql_module, database_param>(name)
		{}

	public:
		virtual bool init() override
		{
			auto pool_size = configs().pool_size;

			for (int i = 0; i < pool_size; ++i)
			{
				auto connector_ptr = std::make_shared<sql_connector>(configs());
				connectors_.push_back(connector_ptr);
			}

			return true;
		}

		virtual auto run(io_context& io) -> awaitable<void> override
		{
			for (auto& connector : connectors_)
			{
				if (!connector)
					continue;

				co_await connector->start();
			}
		}

		template <typename T>
		auto async_query(std::string_view sql) -> awaitable<std::vector<T>>
		{
			auto connector = get_connector();

			co_return co_await connector->async_query(sql);
		}

		auto async_execute(std::string_view sql) -> awaitable<std::size_t>
		{
			auto connector = get_connector();

			co_return co_await connector->async_execute(sql);
		}

	private:
		std::shared_ptr<sql_connector> get_connector()
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
		std::size_t index_;

		std::mutex mutex_;

		std::vector<std::shared_ptr<sql_connector>> connectors_;
	};
} // namespace aquarius