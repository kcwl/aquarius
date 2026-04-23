#define BOOST_TEST_NO_MAIN
#include <aquarius/http.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

namespace aquarius
{
	HTTP_CONFIG_INVOKE(cfg)
	{
		cfg.control_allow_headers = "";
	}
}


BOOST_AUTO_TEST_SUITE(ut_http)

struct mock_read_error_session
{
	mock_read_error_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		co_return asio::error::connection_aborted;
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	asio::io_context& io;
};

struct mock_error_method_session
{
	mock_error_method_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "HELLO /test_error HTTP/1.1";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run = false;

	asio::io_context& io;
};

struct mock_error_url_session
{
	mock_error_url_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "POST 11?test_error HTTP/1.1";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run = false;
	asio::io_context& io;
};

struct mock_error_version_session
{
	mock_error_version_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "POST /test_error HTTP/2.1";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run = false;
	asio::io_context& io;
};

struct mock_complete_no_recv_session
{
	mock_complete_no_recv_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "POST /test_error HTTP/1.1\r\nContent-Length: 2\r\n\r\n{}";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = 25u;

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run = false;
	asio::io_context& io;
};

struct mock_no_length_session
{
	mock_no_length_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "POST /test_error HTTP/1.1\r\n\r\n";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run = false;
	asio::io_context& io;
};

struct mock_normal_session
{
	mock_normal_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "POST /test_error HTTP/1.1\r\nContent-Length: 2\r\n\r\n";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer& buffer, std::size_t) -> asio::awaitable<error_code>
	{
		std::string content = "{}";
		buffer.sputn(content.c_str(), content.size());
		co_return error_code{};
	}

	bool has_run = false;

	asio::io_context& io;
};

template <typename T>
struct test_acceptor_session
{
	void operator()(error_code ec)
	{
		asio::io_context io{};

		auto session = std::make_shared<T>(io);

		auto future = asio::co_spawn(
			io, [session]() -> asio::awaitable<error_code> { co_return co_await http::accept(session); },
			asio::use_future);

		io.run();

		BOOST_TEST(future.get() == ec);
	}
};

struct mock_query_error_status_session
{
	mock_query_error_status_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "HTTP/1.1 9991 no content";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run = false;
	asio::io_context& io;
};

struct mock_query_error_version_session
{
	mock_query_error_version_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "HTTP/2.1 200 OK";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool has_run = false;
	asio::io_context& io;
};

struct mock_query_normal_session
{
	mock_query_normal_session(asio::io_context& context)
		: io(context)
	{}

	auto get_executor()
	{
		return io.get_executor();
	}

	auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& endpos) -> asio::awaitable<error_code>
	{
		if (has_run)
		{
			co_return asio::error::eof;
		}

		std::string headline = "POST /test_error HTTP/1.1\r\nContent-Length: 2\r\n\r\n";

		buffer.sputn(headline.c_str(), headline.size());

		endpos = headline.size();

		has_run = true;

		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<void>
	{
		co_return;
	}
	auto async_read(flex_buffer& buffer, std::size_t) -> asio::awaitable<error_code>
	{
		std::string content = "{}";
		buffer.sputn(content.c_str(), content.size());
		co_return error_code{};
	}

	bool has_run = false;
	asio::io_context& io;
};

template <typename T>
struct test_query_session
{
	void operator()(error_code ec, const std::string& content = {})
	{
		asio::io_context io{};

		auto session = std::make_shared<T>(io);

		auto future = asio::co_spawn(
			io, [session]() -> asio::awaitable<std::expected<flex_buffer, error_code>>
			{ co_return co_await http::query(session); }, asio::use_future);

		io.run();

		auto result = future.get();

		if (result.has_value())
		{
			BOOST_TEST(std::string((char*)(*result).data().data(), (*result).size()) == content);
		}
		else
		{
			BOOST_TEST(future.get().error() == ec);
		}
	}
};

BOOST_AUTO_TEST_CASE(accepts_read_error)
{
	test_acceptor_session<mock_read_error_session>()(asio::error::connection_aborted);
}

BOOST_AUTO_TEST_CASE(accepts_method_error)
{
	test_acceptor_session<mock_error_method_session>()(asio::error::eof);
}
BOOST_AUTO_TEST_CASE(accepts_version_error)
{
	test_acceptor_session<mock_error_version_session>()(asio::error::eof);
}
BOOST_AUTO_TEST_CASE(accepts_url_error)
{
	test_acceptor_session<mock_error_url_session>()(asio::error::eof);
}
BOOST_AUTO_TEST_CASE(accepts_complete_no_recv)
{
	test_acceptor_session<mock_complete_no_recv_session>()(asio::error::eof);
}
BOOST_AUTO_TEST_CASE(accepts_no_length)
{
	test_acceptor_session<mock_no_length_session>()(asio::error::eof);
}
BOOST_AUTO_TEST_CASE(accepts_normal_flow)
{
	test_acceptor_session<mock_normal_session>()(asio::error::eof);
}

BOOST_AUTO_TEST_CASE(query_read_error)
{
	test_query_session<mock_read_error_session>()(asio::error::eof);
}

BOOST_AUTO_TEST_CASE(query_version_error)
{
	test_query_session<mock_query_error_version_session>()(http_status::http_version_not_supported);
}
BOOST_AUTO_TEST_CASE(query_status_error)
{
	test_query_session<mock_query_error_status_session>()(http_status::internal_server_error);
}
BOOST_AUTO_TEST_CASE(query_normal_flow)
{
	test_query_session<mock_query_normal_session>()(http_status::ok,"");
}

BOOST_AUTO_TEST_SUITE_END()