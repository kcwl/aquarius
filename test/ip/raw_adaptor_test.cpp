#define BOOST_TEST_NO_MAIN
#include <aquarius/ip/adaptor/raw_adaptor.hpp>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <thread>

using namespace aquarius;
using namespace std::chrono_literals;

namespace boost
{
	namespace asio
	{
		template<typename Socket, typename Endpoint, typename Token>
		auto async_connect(Socket&, Endpoint, Token) -> asio::awaitable<error_code>
		{
			co_return error_code{};
		}
	}
}

struct mock_socket
{};

BOOST_AUTO_TEST_SUITE(ut_raw_adaptor)

BOOST_AUTO_TEST_CASE(ctor)
{
	mock_socket socket;
	raw_adaptor<mock_socket> adaptor(socket);

	BOOST_CHECK_EQUAL(socket, adaptor.get_implement());
}

BOOST_AUTO_TEST_CASE(connect)
{
	mock_socket socket;
	raw_adaptor<mock_socket> adaptor(socket);

	asio::io_context io{};

	auto future = asio::co_spawn(
		io,
		[&]() -> asio::awaitable<void>
		{
			asio::ip::tcp::resolver resolve{ io };

			auto ec = co_await adaptor.async_connect(resolve.resolve({}, {}), 1s);
			BOOST_TEST(!ec);
		},
		asio::use_future);
}

BOOST_AUTO_TEST_CASE(handshake)
{
	mock_socket socket;
	raw_adaptor<mock_socket> adaptor(socket);

	asio::io_context io{};

	auto future = asio::co_spawn(
		io,
		[&]() -> asio::awaitable<void>
		{
			asio::ip::tcp::resolver resolve{ io };

			auto ec = co_await adaptor.async_handshake(1s);
			BOOST_TEST(!ec);
		},
		asio::use_future);
}

BOOST_AUTO_TEST_SUITE_END()