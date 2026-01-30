#define BOOST_TEST_NO_MAIN
#include <aquarius/asio.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>
#include <aquarius/module/session_module.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(session_store)

BOOST_AUTO_TEST_CASE(store)
{
	aquarius::io_context io;

	auto future = aquarius::co_spawn(
		io,
		[&] -> aquarius::awaitable<void>
		{
			auto session_ptr =
				std::make_shared<aquarius::tcp_server_session>(std::move(boost::asio::ip::tcp::socket(io)), 30ms, 3s);

			auto uid = session_ptr->uuid();

			co_await aquarius::mpc_insert_session<aquarius::tcp_server_session>(session_ptr);

			// BOOST_TEST(aquarius::detail::session_storage<aquarius::tcp_server_session>() == 1);

			auto new_session_ptr = co_await aquarius::mpc_find_session<aquarius::tcp_server_session>(uid);

			BOOST_TEST(!!new_session_ptr);

			co_await aquarius::mpc_erase_session<aquarius::tcp_server_session>(uid);

			auto new_session_ptr_1 = co_await aquarius::mpc_find_session<aquarius::tcp_server_session>(uid);

			BOOST_TEST(!new_session_ptr_1);
		},
		aquarius::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(multi_regist)
{
	aquarius::io_context io;

	auto future = aquarius::co_spawn(
		io,
		[&] -> aquarius::awaitable<void>
		{
			auto session_ptr =
				std::make_shared<aquarius::tcp_server_session>(std::move(boost::asio::ip::tcp::socket(io)), 30ms, 3s);

			BOOST_TEST(co_await aquarius::mpc_insert_session<aquarius::tcp_server_session>(session_ptr));

			BOOST_TEST(!co_await aquarius::mpc_insert_session<aquarius::tcp_server_session>(session_ptr));
		},
		aquarius::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_SUITE_END()