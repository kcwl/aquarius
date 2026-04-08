#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#ifdef ENABLE_MYSQL
#include <aquarius/tbl/detail/mysql_io_service.hpp>
#endif

namespace aquarius
{
#ifdef ENABLE_MYSQL
	template <typename Executor>
	using sql_op = mysql_io_service<Executor>;
#endif
} // namespace aquarius