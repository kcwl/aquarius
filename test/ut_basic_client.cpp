#include <aquarius/basic_client.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_client)

template <typename Session>
class mock_ctor_client : public basic_client<Session>
{
public:
	mock_ctor_client(asio::io_context& context, std::chrono::milliseconds timeout)
		: basic_client<Session>(context, timeout)
	{}

public:
	bool check_ctor(asio::io_context& context, std::chrono::milliseconds timeout)
	{
		return context.get_executor() == this->get_executor() && timeout == this->timeout_;
	}
};

using namespace std::chrono_literals;

struct mock_session
{
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;
};
BOOST_AUTO_TEST_CASE(ctor)
{
	asio::io_context io{};
	auto timeout = 10ms;
	mock_ctor_client<mock_session> cli(io, timeout);

	BOOST_TEST(cli.check_ctor(io, timeout));
}

class mock_connect_failed_protocol
{
public:
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

public:
	mock_connect_failed_protocol(socket, std::chrono::milliseconds)
	{}

public:
	auto async_connect(const std::string& host, const std::string& port) -> asio::awaitable<error_code>
	{
		co_return boost::asio::error::bad_descriptor;
	}

	auto query() -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	bool shutdown()
	{
		return true;
	}
};

BOOST_AUTO_TEST_CASE(client_async_connect_failed)
{
	asio::io_context io{};
	auto timeout = 10ms;

	auto cli = std::make_shared<basic_client<mock_connect_failed_protocol>>(io, timeout);

	auto future = asio::co_spawn(
		io,
		[cli]() -> asio::awaitable<void>
		{
			auto ec = co_await cli->async_connect("", 0);

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	future.get();
}

class mock_connect_success_protocol
{
public:
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

public:
	mock_connect_success_protocol(socket, std::chrono::milliseconds)
	{}

	auto async_connect(const std::string&, const std::string&) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	auto query() -> asio::awaitable<error_code>
	{
		co_return boost::asio::error::bad_descriptor;
	}

	void close()
	{}
};

BOOST_AUTO_TEST_CASE(client_async_connect_success_and_query_failed)
{
	asio::io_context io{};
	auto timeout = 10ms;

	auto cli = std::make_shared<basic_client<mock_connect_failed_protocol>>(io, timeout);

	cli->set_close_func([](auto session) { BOOST_TEST(session); });

	auto future = asio::co_spawn(
		io,
		[cli]() -> asio::awaitable<void>
		{
			auto ec = co_await cli->async_connect("", 0);

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(client_invoke_accept_and_close)
{
	asio::io_context io{};
	auto timeout = 10ms;

	basic_client<mock_session> cli(io, timeout);

	cli.set_accept_func([](auto) { BOOST_TEST(true); });
	cli.set_close_func([](auto) { BOOST_TEST(true); });

	cli.accept_invoke();
	cli.close_invoke();
}

class mock_send_request_failed_protocol
{
public:
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

public:
	mock_send_request_failed_protocol(socket, std::chrono::milliseconds)
	{}

	template <typename Request, typename Func>
	auto send_request(std::shared_ptr<Request>, Func&&, error_code& ec) -> asio::awaitable<std::size_t>
	{
		ec = boost::asio::error::bad_descriptor;
		co_return 0;
	}

	auto wait(std::size_t) -> asio::awaitable<void>
	{
		co_return;
	}

	bool shutdown()
	{
		return true;
	}
};

class mock_send_request_success_protocol
{
public:
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

public:
	mock_send_request_success_protocol(socket, std::chrono::milliseconds)
	{}

	template <typename Request, typename Func>
	auto send_request(std::shared_ptr<Request>, Func&& f, error_code& ec) -> asio::awaitable<std::size_t>
	{
		flex_buffer buffer{};
		f(buffer, "");
		ec = error_code{};
		co_return 0;
	}

	auto wait(std::size_t) -> asio::awaitable<void>
	{
		co_return;
	}

	bool shutdown()
	{
		return true;
	}
};

struct mock_request
{};
struct mock_response
{
	mock_response()
		: value(0)
	{}

	void consume(flex_buffer&)
	{
		value = 1;
	}

	int value;
};

BOOST_AUTO_TEST_CASE(async_call_request_falied)
{
	asio::io_context io{};
	auto timeout = 10ms;

	auto cli = std::make_shared<basic_client<mock_send_request_failed_protocol>>(io, timeout);

	auto future = asio::co_spawn(
		io,
		[cli]() -> asio::awaitable<void>
		{
			auto req = std::make_shared<mock_request>();
			auto resp = co_await cli->async_call<mock_response>(req);

			BOOST_TEST(resp.value == 0);
		},
		asio::use_future);
}

BOOST_AUTO_TEST_CASE(async_call_request_success)
{
	asio::io_context io{};
	auto timeout = 10ms;

	auto cli = std::make_shared<basic_client<mock_send_request_success_protocol>>(io, timeout);

	auto future = asio::co_spawn(
		io,
		[cli]() -> asio::awaitable<void>
		{
			auto req = std::make_shared<mock_request>();
			auto resp = co_await cli->async_call<mock_response>(req);

			BOOST_TEST(resp.value == 1);
		},
		asio::use_future);
}

class mock_send_buffer_failed_protocol
{
public:
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

public:
	mock_send_buffer_failed_protocol(socket, std::chrono::milliseconds)
	{}

	template <typename Func>
	auto send_buffer(flex_buffer&, const std::string&, Func&& f, error_code& ec) -> asio::awaitable<std::size_t>
	{
		ec = boost::asio::error::bad_descriptor;
		co_return 0;
	}

	template <typename ConstBufferSequece, typename Func>
	auto send_buffers(ConstBufferSequece&&, Func&& f, error_code& ec) -> asio::awaitable<std::size_t>
	{
		ec = boost::asio::error::bad_descriptor;
		co_return 0;
	}

	auto wait(std::size_t) -> asio::awaitable<void>
	{
		co_return;
	}

	bool shutdown()
	{
		return true;
	}
};

BOOST_AUTO_TEST_CASE(async_call_buffer_falied)
{
	asio::io_context io{};
	auto timeout = 10ms;

	auto cli = std::make_shared<basic_client<mock_send_buffer_failed_protocol>>(io, timeout);

	auto future = asio::co_spawn(
		io,
		[cli]() -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await cli->async_call_buffer(
				buffer, std::string{},
				[](flex_buffer& buffer, const std::string&) -> asio::awaitable<error_code>
				{
					BOOST_TEST(false);
					co_return error_code{};
				});

			BOOST_TEST(ec);
		},
		asio::use_future);
}

class mock_send_buffer_success_protocol
{
public:
	using socket = asio::ip::tcp::socket;
	using resolver = asio::ip::tcp::resolver;

public:
	mock_send_buffer_success_protocol(socket, std::chrono::milliseconds)
	{}

	template <typename Func>
	auto send_buffer(flex_buffer&,const std::string& r, Func&& f, error_code& ec) -> asio::awaitable<std::size_t>
	{
		flex_buffer buffer{};
		f(buffer,r);
		ec = error_code{};
		co_return 0;
	}

	template <typename ConstBufferSequece, typename Func>
	auto send_buffers(ConstBufferSequece&&, Func&& f, error_code& ec) -> asio::awaitable<std::size_t>
	{
		ec = boost::asio::error::bad_descriptor;
		co_return 0;
	}

	auto wait(std::size_t) -> asio::awaitable<void>
	{
		co_return;
	}

	bool shutdown()
	{
		return true;
	}
};

BOOST_AUTO_TEST_CASE(async_call_buffer_success)
{
	asio::io_context io{};
	auto timeout = 10ms;

	auto cli = std::make_shared<basic_client<mock_send_buffer_success_protocol>>(io, timeout);

	auto future = asio::co_spawn(
		io,
		[cli]() -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await cli->async_call_buffer(
				buffer,"",
				[](flex_buffer& buffer, const std::string&) -> asio::awaitable<error_code>
				{
					BOOST_TEST(true);
					co_return error_code{};
				});

			BOOST_TEST(!ec);
		},
		asio::use_future);
}

BOOST_AUTO_TEST_SUITE_END()
