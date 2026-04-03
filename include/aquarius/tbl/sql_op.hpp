#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#ifdef MYSQL_OP
#include <aquarius/tbl/detail/mysql_io_service.hpp>
#endif

namespace aquarius
{
#ifdef MYSQL_OP
	template <typename Executor>
	using sql_op = mysql_io_service<Executor>;
#endif
} // namespace aquarius