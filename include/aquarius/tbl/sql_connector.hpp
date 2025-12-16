#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/tbl/database_param.hpp>
#include <aquarius/tbl/sql_error.hpp>
#include <aquarius/tbl/transaction.hpp>
#include <expected>

#if defined(MYSQL_SQL)
#include <aquarius/tbl/mysql.hpp>
#else
#include <aquarius/tbl/empty.hpp>
#endif

namespace aquarius
{
	class sql_connector
	{
#if defined(MYSQL_SQL)
		using service_impl = sql::mysql;
#else
		using service_impl = sql::empty_impl;
#endif

	public:
		sql_connector(const database_param& param)
			: param_(param)
			, service_(param_)
			, completed_(false)
		{
			
		}

	public:
		auto start() -> awaitable<bool>
		{
			co_return co_await async_connect();
		}

		auto async_connect() -> awaitable<bool>
		{
			error_code ec = co_await service_.async_connect();

			if (ec)
			{
				XLOG_ERROR() << "sql connect " << param_.host << " failed! error: " << ec.what();
			}

			co_return !ec;
		}

		template <typename T>
		auto async_query(std::string_view sql) -> awaitable<T>
		{
			error_code ec{};

			auto result =
				co_await service_.template async_query<typename T::value_type>(sql, ec);

			if (ec)
				co_return T{};

			completed_ = !ec;

			co_return result;
		}

		auto async_execute(std::string_view sql) -> awaitable<std::size_t>
		{
			sql::trans_guard lk(this);

			error_code ec{};

			auto result = co_await service_.async_execute(sql, ec);

			completed_ = !ec;

			co_return ec ? 0 : result;
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
		database_param param_;

		service_impl service_;

		bool completed_;
	};
} // namespace aquarius