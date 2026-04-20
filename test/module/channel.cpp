#define BOOST_TEST_NO_MAIN
#include <aquarius/module/handler_channel.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_channel)

BOOST_AUTO_TEST_CASE(normal_flow)
{
	asio::io_context io{};

	flex_buffer buf{};
	std::string content = "normal channel flow";
	buf.sputn(content.c_str(), content.size());

	auto f = [](flex_buffer& buffer, int) -> asio::awaitable<std::expected<flex_buffer, error_code>>
	{
		flex_buffer resp;
		resp.sputn((char*)buffer.data().data(), buffer.size());
		co_return resp;
	};

	mpc_subscribe("test", f);

	auto future = asio::co_spawn(
		io, [&]() -> asio::awaitable<std::expected<flex_buffer, error_code>>
		{ co_return co_await mpc_publish("test", buf, 0); }, asio::use_future);

	io.run();

	auto cnt = *future.get();

	BOOST_TEST(std::string((char*)cnt.data().data(), cnt.size()) == content);
}

BOOST_AUTO_TEST_CASE(error_flow)
{
	asio::io_context io{};

	flex_buffer buf{};

	auto f = [](flex_buffer& buffer, int) -> asio::awaitable<std::expected<flex_buffer, error_code>>
	{ co_return std::unexpected(asio::error::bad_descriptor); };

	mpc_subscribe("test1", f);

	auto future = asio::co_spawn(
		io, [&]() -> asio::awaitable<std::expected<flex_buffer, error_code>>
		{ co_return co_await mpc_publish("test1", buf, 0); }, asio::use_future);

	io.run();

	auto cnt = future.get().error();

	BOOST_TEST(cnt = asio::error::bad_descriptor);
}

BOOST_AUTO_TEST_CASE(publish_no_exist)
{
	asio::io_context io{};

	flex_buffer buf{};

	auto future = asio::co_spawn(
		io, [&]() -> asio::awaitable<std::expected<flex_buffer, error_code>>
		{ co_return co_await mpc_publish("test2", buf, 0); }, asio::use_future);

	io.run();

	auto cnt = future.get().error();

	BOOST_TEST(cnt == error_code{});
}

BOOST_AUTO_TEST_CASE(multi_subscribe)
{
	auto f = [](flex_buffer& buffer, int) -> asio::awaitable<std::expected<flex_buffer, error_code>>
	{ co_return std::unexpected(asio::error::bad_descriptor); };

	auto result = mpc_subscribe("test", f);

	BOOST_TEST(!result);
}

BOOST_AUTO_TEST_SUITE_END()