#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

using namespace std::chrono_literals;
using namespace aquarius;


struct mock_protocol
{
	using socket = asio::ip::tcp::socket;
	using endpoint = asio::ip::tcp::endpoint;
	using acceptor = asio::ip::tcp::acceptor;
	using resolver = asio::ip::tcp::resolver;
	using no_delay = asio::ip::tcp::no_delay;

	using keep_alive = asio::socket_base::keep_alive;

	static auto query(auto) -> asio::awaitable<std::expected<flex_buffer, error_code>>
	{
		flex_buffer buffer{};
		co_return buffer;
	}

	static auto accept(auto) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
};

template <typename T>
struct mock_adaptor
{
	mock_adaptor(asio::ip::tcp::socket& sock)
		: socket_(sock)
	{

	}

	auto async_handshake(auto) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
	static auto accept(auto) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	auto& get_implement()
	{
		return socket_;
	}

	template<typename Endpoint, typename Dura>
	auto async_connect(Endpoint, Dura) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	asio::ip::tcp::socket& socket_;
};

struct mock_error_protocol
{
	using socket = asio::ip::tcp::socket;
	using endpoint = asio::ip::tcp::endpoint;
	using acceptor = asio::ip::tcp::acceptor;
	using resolver = asio::ip::tcp::resolver;

	static auto query(auto) -> asio::awaitable<std::expected<flex_buffer, error_code>>
	{
		flex_buffer buffer{};
		co_return buffer;
	}

	static auto accept(auto) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
};

template <typename T>
struct mock_error_adaptor
{
	mock_error_adaptor(asio::ip::tcp::socket& sock)
		: socket_(sock)
	{

	}

	auto async_handshake(auto) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
	static auto accept(auto) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	auto& get_implement()
	{
		return socket_;
	}

	template<typename Endpoint, typename Dura>
	auto async_connect(Endpoint, Dura) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	auto async_send(flex_buffer) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	asio::ip::tcp::socket& socket_;
};

BOOST_AUTO_TEST_SUITE(sessions)

BOOST_AUTO_TEST_CASE(flow_success)
{
	asio::io_context io{};

	using session_t = basic_session<mock_protocol, mock_adaptor>;

	typename session_t::socket socket(io);

	auto session = std::make_shared<session_t>(std::move(socket), 1s);

	BOOST_CHECK_THROW(session->remote_address(), std::exception);
	BOOST_CHECK_THROW(session->remote_address_u(), std::exception);
	BOOST_CHECK_THROW(session->remote_port(), std::exception);

	BOOST_TEST(session->close());

	BOOST_TEST(!session->keep_alive());

	BOOST_TEST(!session->set_nodelay());

	auto future = asio::co_spawn(
		io,
		[&, session]() -> asio::awaitable<void>
		{
			BOOST_TEST(co_await session->async_connect("", "") == error_code{});

			flex_buffer buffer{};
			std::size_t endpos{};
			BOOST_TEST((co_await session->async_read(buffer, 10)) == error_code{});

			BOOST_TEST(co_await session->async_read_util(buffer, "\r\n", endpos) == error_code{});

			BOOST_TEST(co_await session->async_send(std::move(buffer)) == error_code{});

			BOOST_TEST(co_await session->accept() == error_code{});

			auto result = co_await session->query();

			if (!result.has_value())
			{
				BOOST_TEST(false);
			}

			BOOST_TEST((*result).size() == 0);
		},
		asio::use_future);
}

BOOST_AUTO_TEST_CASE(flow_failed)
{
	using session_t = basic_session<mock_error_protocol, mock_error_adaptor>;

	asio::io_context io{};

	typename session_t::socket socket(io);

	auto session = std::make_shared<session_t>(std::move(socket), 1s);

	auto future = asio::co_spawn(
		io,
		[&]() -> asio::awaitable<void>
		{
			BOOST_CHECK_THROW(co_await session->async_connect("", ""), std::exception);

			flex_buffer buffer{};
			std::size_t endpos{};
			BOOST_TEST(co_await session->async_read(buffer, 10) == asio::error::eof);

			BOOST_TEST(co_await session->async_read_util(buffer, "\r\n", endpos) == asio::error::eof);

			BOOST_TEST(co_await session->async_send(std::move(buffer)) == asio::error::eof);

			BOOST_TEST(co_await session->accept() == asio::error::eof);

			auto result = co_await session->query();

			if (result.has_value())
			{
				BOOST_TEST(false);
			}

			BOOST_TEST(result.error() == asio::error::eof);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_SUITE_END()