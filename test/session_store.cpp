#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>
#include <aquarius/session_store.hpp>

BOOST_AUTO_TEST_SUITE(session_store)

BOOST_AUTO_TEST_CASE(store)
{
	aquarius::io_context io;
	auto session_ptr = std::make_shared<aquarius::tcp_server_session>(std::move(aquarius::ip::tcp::socket(io)));

	auto uid = session_ptr->uuid();

	aquarius::regist_session(session_ptr);

	//BOOST_TEST(aquarius::detail::session_storage<aquarius::tcp_server_session>() == 1);

	auto new_session_ptr = aquarius::attach_session<aquarius::tcp_server_session>(uid);

	BOOST_TEST(new_session_ptr);

	aquarius::remove_session<aquarius::tcp_server_session>(uid);

	auto new_session_ptr_1 = aquarius::attach_session<aquarius::tcp_server_session>(uid);

	BOOST_TEST(!new_session_ptr_1);
}

BOOST_AUTO_TEST_CASE(multi_regist)
{
	aquarius::io_context io;

	auto session_ptr = std::make_shared<aquarius::tcp_server_session>(std::move(aquarius::ip::tcp::socket(io)));

	BOOST_TEST(aquarius::regist_session(session_ptr));

	BOOST_TEST(!aquarius::regist_session(session_ptr));
}

BOOST_AUTO_TEST_SUITE_END()