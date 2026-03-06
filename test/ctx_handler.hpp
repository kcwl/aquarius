#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>
#include "test.virgo.h"
#include <boost/test/unit_test.hpp>

using namespace std::string_view_literals;

AQUARIUS_HANDLER(login_request, login_response, ctx_tcp_test)
{
	response().header().uuid(request()->header().uuid());

	response().body().per_resp = request()->body().per_req;

	co_return aquarius::error_code{};
}


AQUARIUS_HANDLER(new_http_login_request, new_http_login_response, ctx_test_http_hander)
{
	response().body().per_resp = request()->body().per_req;

	co_return aquarius::virgo::http_status::ok;
}

AQUARIUS_HANDLER(http_test_get_request, http_test_get_response, ctx_http_test_get_request)
{
	if (!request())
	{
		co_return aquarius::virgo::http_status::bad_request;
	}

	response().body().user = request()->body().user;

	response().body().passwd = request()->body().passwd;

	co_return aquarius::virgo::http_status::ok;
}

template<typename Person>
void check_person(const Person& lhs, const Person& rhs)
{
	BOOST_TEST(lhs.addr == rhs.addr);
	BOOST_TEST(lhs.age == rhs.age);
	BOOST_TEST(lhs.hp == rhs.hp);
	BOOST_TEST(lhs.mana == rhs.mana);
	BOOST_TEST(lhs.name == rhs.name);
	BOOST_TEST(lhs.orders == lhs.orders);
	BOOST_TEST(lhs.score == rhs.score);
	BOOST_TEST(lhs.sex == rhs.sex);
	BOOST_TEST(lhs.telephone == rhs.telephone);
}