#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius.hpp>

using namespace aquarius;

template<typename Protocol, typename Executor = asio::any_io_executor>
struct mock_socket : public asio::basic_socket<Protocol, Executor>
{
	mock_socket(asio::io_context& io)
		: asio::basic_socket<Protocol, Executor>(io)
	{}
	struct initiate_async_connect
	{
		initiate_async_connect(initiate_async_connect*) {}
	};

	template<typename Token, typename Endpoint>
	auto async_connect(const Token& peer_endpoint,
					   Token&& token)
		-> decltype(
			asio::async_initiate<Token, void(error_code)>(
				std::declval<initiate_async_connect>(), token,
				peer_endpoint, std::declval<error_code&>()))
	{
		boost::system::error_code open_ec;
		return async_initiate<Token, void(boost::system::error_code)>(
			initiate_async_connect(this), token, peer_endpoint, open_ec);
	}

	template<typename Buffer, typename Token>
	auto async_read_some(Buffer, Token) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}

	template<typename Buffer, typename Token>
	auto async_write_some(Buffer, Token) -> asio::awaitable<error_code>
	{
		co_return error_code{};
	}
};

BOOST_AUTO_TEST_SUITE(client_ssls)

BOOST_AUTO_TEST_CASE(ctor)
{
	asio::io_context io{};
	mock_socket<asio::ip::tcp> socket(io);
	ssl_client_adaptor<mock_socket<asio::ip::tcp>> adaptor(socket);

	auto result = socket.get_executor() == adaptor.get_implement().get_executor();
	BOOST_TEST(result);
}

BOOST_AUTO_TEST_CASE(connect)
{
	asio::io_context io{};
	mock_socket<asio::ip::tcp> socket(io);
	ssl_client_adaptor<mock_socket<asio::ip::tcp>> adaptor(socket);

	auto future = asio::co_spawn(
		io,
		[&] () -> asio::awaitable<void>
		{
			asio::ip::tcp::resolver resolve{ io };

			auto ec = co_await adaptor.async_connect(resolve.resolve({}, {}), 1s);
			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(handshake)
{
	asio::io_context io{};
	mock_socket<asio::ip::tcp> socket(io);
	ssl_client_adaptor<mock_socket<asio::ip::tcp>> adaptor(socket);

	auto future = asio::co_spawn(
		io,
		[&] () -> asio::awaitable<void>
		{
			asio::ip::tcp::resolver resolve{ io };

			auto ec = co_await adaptor.async_handshake(1s);
			BOOST_TEST(!ec);
		},
		asio::use_future);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(server_ssls)

BOOST_AUTO_TEST_CASE(ctor)
{
	asio::io_context io{};
	mock_socket<asio::ip::tcp> socket(io);
	ssl_server_adaptor<mock_socket<asio::ip::tcp>> adaptor(socket);

	auto result = socket.get_executor() == adaptor.get_implement().get_executor();
	BOOST_TEST(result);
}

BOOST_AUTO_TEST_CASE(connect)
{
	asio::io_context io{};
	mock_socket<asio::ip::tcp> socket(io);
	ssl_server_adaptor<mock_socket<asio::ip::tcp>> adaptor(socket);

	auto future = asio::co_spawn(
		io,
		[&] () -> asio::awaitable<void>
		{
			asio::ip::tcp::resolver resolve{ io };

			auto ec = co_await adaptor.async_connect(resolve.resolve({}, {}), 1s);
			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(handshake)
{
	asio::io_context io{};
	mock_socket<asio::ip::tcp> socket(io);
	ssl_server_adaptor<mock_socket<asio::ip::tcp>> adaptor(socket);

	auto future = asio::co_spawn(
		io,
		[&] () -> asio::awaitable<void>
		{
			asio::ip::tcp::resolver resolve{ io };

			auto ec = co_await adaptor.async_handshake(1s);
			BOOST_TEST(!ec);
		},
		asio::use_future);
}

BOOST_AUTO_TEST_SUITE_END()
