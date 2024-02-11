#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(session)

BOOST_AUTO_TEST_CASE(session_null)
{
	auto session_ptr = aquarius::find_session(1001);

	BOOST_CHECK(session_ptr == nullptr);

	auto sessions = aquarius::find_session_if([](auto session_ptr)
		{
			return session_ptr->uuid() == 1001;
		});

	BOOST_CHECK(sessions.empty());

	BOOST_CHECK(aquarius::erase_session(1001));
}

BOOST_AUTO_TEST_CASE(session_full)
{
	auto session = std::make_shared<aquarius::session<aquarius::connect<aquarius::tcp,aquarius::conn_mode::server,aquarius::ssl_mode::ssl>>>(nullptr);

	auto session_1 = std::make_shared<aquarius::session<aquarius::connect<aquarius::tcp, aquarius::conn_mode::server, aquarius::ssl_mode::ssl>>>(nullptr);

	aquarius::session_manager::instance().push(session);
	aquarius::session_manager::instance().push(session_1);


}

BOOST_AUTO_TEST_SUITE_END()