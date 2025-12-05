#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/server_session.hpp>
#include <aquarius/detail/session_store.hpp>

BOOST_AUTO_TEST_SUITE(session_store)

BOOST_AUTO_TEST_CASE(store)
{
	aquarius::io_context io;
	auto session_ptr = std::make_shared<aquarius::server_session<aquarius::protocol_tag::tcp>>(std::move(aquarius::ip::tcp::socket(io)));

	auto uid = session_ptr->uuid();

	aquarius::detail::regist_session(session_ptr);

	//BOOST_TEST(aquarius::detail::session_storage<aquarius::tcp_server_session>() == 1);

	auto new_session_ptr = aquarius::detail::attach_session<aquarius::server_session<aquarius::protocol_tag::tcp>>(uid);

	BOOST_TEST(new_session_ptr);

	aquarius::detail::remove_session<aquarius::server_session<aquarius::protocol_tag::tcp>>(uid);

	auto new_session_ptr_1 = aquarius::detail::attach_session<aquarius::server_session<aquarius::protocol_tag::tcp>>(uid);

	BOOST_TEST(!new_session_ptr_1);
}

BOOST_AUTO_TEST_CASE(multi_regist)
{
	aquarius::io_context io;

	auto session_ptr = std::make_shared<aquarius::server_session<aquarius::protocol_tag::tcp>>(std::move(aquarius::ip::tcp::socket(io)));

	BOOST_TEST(aquarius::detail::regist_session(session_ptr));

	BOOST_TEST(!aquarius::detail::regist_session(session_ptr));
}

BOOST_AUTO_TEST_SUITE_END()