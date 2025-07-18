#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/any_io_executor.hpp>
#include <aquarius/awaitable.hpp>
#include <aquarius/basic_attribute.hpp>
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_context.hpp>
#include <aquarius/basic_router.hpp>
#include <aquarius/basic_server.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/basic_sql_stream.hpp>
#include <aquarius/co_spawn.hpp>
#include <aquarius/concepts.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler.hpp>
#include <aquarius/context/stream_context.hpp>
#include <aquarius/context/stream_handler.hpp>
#include <aquarius/context/transfer_context.hpp>
#include <aquarius/crc.hpp>
#include <aquarius/deadline_timer.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/error.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_context.hpp>
#include <aquarius/ip/address.hpp>
#include <aquarius/ip/tcp.hpp>
#include <aquarius/ip/tcp_client.hpp>
#include <aquarius/ip/tcp_server.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/mysql_pool.hpp>
#include <aquarius/noncopyable.hpp>
#include <aquarius/post.hpp>
#include <aquarius/redirect_error.hpp>
#include <aquarius/signal_set.hpp>
#include <aquarius/singleton.hpp>
#include <aquarius/ssl/ssl.hpp>
#include <aquarius/ssl/ssl_context_factory.hpp>
#include <aquarius/use_awaitable.hpp>
#include <aquarius/use_future.hpp>
