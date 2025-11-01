#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/http/http_router.hpp>
#include <aquarius/ip/tcp/tcp_router.hpp>
#include "ctx_handler.hpp"

BOOST_AUTO_TEST_SUITE(routers)

BOOST_AUTO_TEST_CASE(http_router)
{
	{
		aquarius::io_context io{};
		auto session = std::make_shared<aquarius::http_server_session>(io);

		aquarius::virgo::http_fields hf;

		aquarius::flex_buffer buffer;

		auto res = aquarius::http_router<aquarius::http_server_session>::get_mutable_instance().invoke(
			http_login_request::method, http_login_request::router, session, hf, std::move(buffer));

		BOOST_TEST(res);
	}

	{
		aquarius::io_context io{};
		auto session = std::make_shared<aquarius::http_server_session>(io);

		aquarius::virgo::http_fields hf;

		aquarius::flex_buffer buffer;

		auto res = aquarius::http_router<aquarius::http_server_session>::get_mutable_instance().invoke(
			aquarius::virgo::http_method::none, http_login_request::router, session, hf, std::move(buffer));

		BOOST_TEST(!res);
	}

	{
		aquarius::io_context io{};
		auto session = std::make_shared<aquarius::http_server_session>(io);

		aquarius::virgo::http_fields hf;

		aquarius::flex_buffer buffer;

		auto res = aquarius::http_router<aquarius::http_server_session>::get_mutable_instance().invoke(
			aquarius::virgo::http_method::get, http_login_request::router, session, hf, std::move(buffer));

		BOOST_TEST(!res);
	}

	{
		aquarius::io_context io{};
		auto session = std::make_shared<aquarius::http_server_session>(io);

		aquarius::virgo::http_fields hf;

		aquarius::flex_buffer buffer;

		auto res = aquarius::http_router<aquarius::http_server_session>::get_mutable_instance().invoke(
			http_login_request::method, "hello", session, hf, std::move(buffer));

		BOOST_TEST(!res);
	}
}

BOOST_AUTO_TEST_CASE(tcp_router)
{
	{
		aquarius::io_context io{};
		auto session = std::make_shared<aquarius::tcp_server_session>(io);

		aquarius::flex_buffer buffer;

		auto res = aquarius::tcp_router<aquarius::tcp_server_session>::get_mutable_instance().invoke(
			login_request::router, session, buffer);

		BOOST_TEST(res);
	}

	//{
	//	aquarius::io_context io{};
	//	auto session = std::make_shared<aquarius::tcp_server_session>(io);

	//	aquarius::flex_buffer buffer;

	//	auto res = aquarius::tcp_router<aquarius::tcp_server_session>::get_mutable_instance().invoke(
	//		login_request::router, session, buffer);

	//	BOOST_TEST(res);
	//}

	//{
	//	aquarius::io_context io{};
	//	auto session = std::make_shared<aquarius::tcp_server_session>(io);

	//	aquarius::flex_buffer buffer;

	//	auto res = aquarius::tcp_router<aquarius::tcp_server_session>::get_mutable_instance().invoke(
	//		login_request::router, session, buffer);

	//	BOOST_TEST(!res);
	//}
}

BOOST_AUTO_TEST_SUITE_END()