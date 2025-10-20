#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/detail/crc.hpp>
#include <aquarius/detail/io_service_pool.hpp>
#include <aquarius/detail/make_endpoint.hpp>
#include <aquarius/detail/session_store.hpp>
#include <aquarius/detail/trie.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/ip/tcp/tcp_server_session.hpp>
#include <string_view>
#include <csignal>

using namespace std::string_view_literals;

BOOST_AUTO_TEST_SUITE(details)

BOOST_AUTO_TEST_CASE(io_service_pools)
{
	{
		BOOST_CHECK_THROW(aquarius::detail::io_service_pool pool(0), std::runtime_error);
	}
	
	{
		aquarius::detail::io_service_pool pool(1);

		auto& io1 = pool.get_io_service();
		auto& io2 = pool.get_io_service();

		BOOST_CHECK(io1.get_executor() == io2.get_executor());
	}
}

BOOST_AUTO_TEST_CASE(crc)
{
	std::string str("hello world");

	auto result = aquarius::detail::crc32(str.c_str(), str.size());

	BOOST_CHECK_EQUAL(result, (uint32_t)0x0d4a1185);
}

BOOST_AUTO_TEST_CASE(flex_buffer)
{
	aquarius::flex_buffer<char> buffer{};

	auto begin = buffer.get();

	BOOST_CHECK_EQUAL(begin, '\0');

	buffer.consume(8191);

	BOOST_CHECK_THROW(buffer.get(), aquarius::error_code);

	buffer.commit(8192);

	BOOST_CHECK_THROW(buffer.put(1), aquarius::error_code);

	BOOST_CHECK(buffer.empty());
}

BOOST_AUTO_TEST_CASE(endpoint)
{
	auto endpoint = aquarius::detail::make_v6_endpoint(1002);

	BOOST_CHECK_EQUAL(endpoint.protocol().family(), BOOST_ASIO_OS_DEF(AF_INET6));

	BOOST_CHECK_EQUAL(endpoint.port(), 1002);
}

BOOST_AUTO_TEST_CASE(session_store)
{
	aquarius::io_context io;
	auto session_ptr= std::make_shared<aquarius::tcp_server_session>(io);

	auto uid = session_ptr->uuid();

	aquarius::detail::regist_session(session_ptr);

	auto new_session_ptr = aquarius::detail::attach_session<aquarius::tcp_server_session>(uid);

	BOOST_CHECK(new_session_ptr);

	aquarius::detail::remove_session<aquarius::tcp_server_session>(uid);

	auto new_session_ptr_1 = aquarius::detail::attach_session<aquarius::tcp_server_session>(uid);

	BOOST_CHECK(!new_session_ptr_1);
}

BOOST_AUTO_TEST_CASE(tri)
{
	aquarius::detail::trie<std::function<void()>> normal_tre{};

	std::function<void()> leaf = [] () { std::cout << "hello leaf;"; };

	normal_tre.add("11001"sv, leaf);

	normal_tre.add("11"sv, leaf);

	BOOST_CHECK(normal_tre.find("11"sv));

	normal_tre.remove("11001"sv);
	normal_tre.remove("11"sv);

	auto cur = normal_tre.find("11001"sv);

	BOOST_CHECK(!cur);
}

BOOST_AUTO_TEST_CASE(server_signal)
{
	aquarius::tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	std::raise(SIGTERM);

	t.join();

	BOOST_CHECK(!srv.enable());
}

BOOST_AUTO_TEST_SUITE_END()