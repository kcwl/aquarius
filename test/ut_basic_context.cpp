#include <aquarius/basic_context.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_context)

struct mock_protocol
{
	template <typename Handler>
	auto handle_request(std::size_t, flex_buffer&, std::size_t) -> asio::awaitable<error_code>
	{
		co_return boost::asio::error::bad_descriptor;
	}
};

BOOST_AUTO_TEST_CASE(basic_protocol_context_ctor)
{
	basic_protocol_context<mock_protocol, std::size_t> ctx(
		[](basic_protocol_context<mock_protocol, std::size_t>*, mock_protocol*, std::size_t, flex_buffer&,
		   std::size_t) -> asio::awaitable<error_code>
		{
			BOOST_TEST(true);
			co_return error_code{};
		});

	asio::io_context io{};
	auto future = asio::co_spawn(
		io,
		[ctx]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await ctx.complete(nullptr, 0, buffer, 0);

			BOOST_TEST(!ec);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(basic_protocol_router)
{
	basic_protocol_context<mock_protocol, std::size_t> ctx(
		[](basic_protocol_context<mock_protocol, std::size_t>*, mock_protocol*, std::size_t, flex_buffer&,
		   std::size_t) -> asio::awaitable<error_code>
		{
			BOOST_TEST(true);
			co_return error_code{};
		});

	ctx.attach_router("router");

	BOOST_TEST(ctx.router() == "router");
}

struct mock_handler
{};

BOOST_AUTO_TEST_CASE(basic_context_ctor)
{
	basic_context<mock_handler, mock_protocol, std::size_t> ctx{};

	asio::io_context io{};
	auto future = asio::co_spawn(
		io,
		[ctx]() mutable -> asio::awaitable<void>
		{
			flex_buffer buffer{};
			auto ec = co_await ctx.complete(nullptr, 0, buffer, 0);

			BOOST_TEST(ec == boost::asio::error::bad_descriptor);
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_SUITE_END()