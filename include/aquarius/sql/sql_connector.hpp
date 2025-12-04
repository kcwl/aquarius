#pragma once
#include <aquarius/sql/database_param.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/sql/sql_error.hpp>
#include <aquarius/coroutine.hpp>

#include <expected>
#include <aquarius/sql/transaction.hpp>

#if defined(MYSQL_SQL)
#include <aquarius/sql/mysql.hpp>
#else
#include <aquarius/sql/empty.hpp>
#endif

namespace aquarius
{
	template <typename Executor>
	class sql_connector
	{
#if defined(MYSQL_SQL)
		using service_impl = sql::mysql<Executor>;
#else
		using service_impl = sql::empty_impl<Executor>;
#endif

	public:
		sql_connector(const Executor& exector, const database_param& param)
			: executor_(exector)
			, param_(param)
			, service_(executor_, param_)
			, completed_(false)
		{}

		template <typename Context>
		sql_connector(Context& context, const database_param& param)
			: sql_connector(context.get_executor(), param)
		{}

	public:
		Executor get_executor() const
		{
			return executor_;
		}

		auto async_connect() -> awaitable<bool>
		{
			error_code ec{};

			co_await service_.async_connect(redirect_error(use_awaitable, ec));

			co_return !ec;
		}

		template <typename T>
		auto async_select(std::string_view sql) -> awaitable<std::vector<T>>
		{
			error_code ec{};

			auto result =  co_await service_.async_query<T>(sql, redirect_error(use_awaitable, ec));

			if (ec)
				co_return std::vector<T>{};

			completed_ = !ec;

			co_return result;
		}

		auto async_insert(std::string_view sql) ->awaitable<std::size_t>
		{
			co_return co_await async_execute(sql);
		}

		auto async_update(std::string_view sql) -> awaitable<std::size_t>
		{
			co_return co_await async_execute(sql);
		}

		auto async_delete(std::string_view sql) -> awaitable<std::size_t>
		{
			co_return co_await async_execute(sql);
		}

		error_code begin()
		{
			return service_.begin();
		}

		void end()
		{
			completed_ ? service_.commit() : service_.rollback();

			completed_ = false;
		}

	private:
		auto async_execute(std::string_view sql) -> awaitable<std::size_t>
		{

			sql::trans_guard lk(this);

			error_code ec{};

			auto result = co_await service_.async_execute(sql, redirect_error(use_awaitable, ec));

			completed_ = !ec;

			co_return ec ? 0 : result;
		}

	private:
		Executor executor_;

		database_param param_;

		service_impl service_;

		bool completed_;
	};
} // namespace aquarius