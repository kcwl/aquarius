#pragma once
#include <aquarius/config.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace aquarius
{
	using boost::asio::io_context;

	using boost::asio::executor_work_guard;
}