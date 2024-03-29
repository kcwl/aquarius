#pragma once
#include <boost/test/unit_test_suite.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(sconnect)

BOOST_AUTO_TEST_CASE(session)
{
	auto sz = aquarius::invoke_session_helper::size();

	auto session = std::make_shared<aquarius::session<aquarius::connect<aquarius::tcp, aquarius::conn_mode::basic_server, aquarius::ssl_mode::ssl>>>(nullptr);

	aquarius::invoke_session_helper::push(session);

	BOOST_CHECK(aquarius::invoke_session_helper::size() == (sz + 1));

	auto session_cpy = aquarius::invoke_session_helper::find(0);

	BOOST_CHECK(session_cpy);

	aquarius::invoke_session_helper::erase(0);

	BOOST_CHECK(aquarius::invoke_session_helper::size() == sz);
}

BOOST_AUTO_TEST_SUITE_END()