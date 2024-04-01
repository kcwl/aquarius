#pragma once
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(core)

BOOST_AUTO_TEST_CASE(apply)
{
	{
		aquarius::invoke_callback_helper::regist(1, [](std::shared_ptr<person_request> ptr) {});

		std::shared_ptr<person_request> req;

		BOOST_CHECK(aquarius::invoke_callback_helper::apply(2, req) == false);
	}

	{
		aquarius::invoke_callback_helper::regist(1, [](std::shared_ptr<person_request> ptr) {});

		std::shared_ptr<person_response> resp;

		BOOST_CHECK(aquarius::invoke_callback_helper::apply(1, resp) == false);
	}
}

BOOST_AUTO_TEST_SUITE_END()