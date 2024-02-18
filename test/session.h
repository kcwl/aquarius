#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(session)

BOOST_AUTO_TEST_CASE(session_null)
{
	auto session_ptr = aquarius::invoke_session_helper::find(1001);

	BOOST_CHECK(session_ptr == nullptr);

	BOOST_CHECK(aquarius::invoke_session_helper::erase(1001));
}

BOOST_AUTO_TEST_CASE(session_full)
{
	auto session = std::make_shared<aquarius::session<aquarius::connect<aquarius::tcp,aquarius::conn_mode::server,aquarius::ssl_mode::ssl>>>(nullptr);

	auto session_1 = std::make_shared<aquarius::session<aquarius::connect<aquarius::tcp, aquarius::conn_mode::server, aquarius::ssl_mode::ssl>>>(nullptr);

	aquarius::router_session::instance().push(session);
	aquarius::router_session::instance().push(session_1);


}

BOOST_AUTO_TEST_SUITE_END()