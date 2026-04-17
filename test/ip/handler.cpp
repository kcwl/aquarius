#define BOOST_TEST_NO_MAIN
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

using namespace std::chrono_literals;
using namespace aquarius;

struct mock_request
{
	void consume(flex_buffer&)
	{}
	void method(int)
	{}
};

struct mock_response
{
	constexpr static auto resp = "mock response"sv;

	void result(int r)
	{
		_result = r;
	}

	error_code commit(flex_buffer& buffer)
	{
		buffer.sputn(resp.data(), resp.size());

		return error_code{};
	}

	int _result;
};

template <typename Request, typename Response>
class mock_handler : public handler<Request, Response>
{
public:
	mock_handler()
		: handler<Request, Response>("test handler")
	{}

public:
	virtual auto handle() -> asio::awaitable<error_code> override
	{
		co_return error_code{};
	}
};

BOOST_AUTO_TEST_SUITE(ut_handler)

BOOST_AUTO_TEST_CASE(ctor)
{
	mock_handler<mock_request, mock_response> handle;

	BOOST_TEST(handle.name() == "test handler");

	BOOST_TEST(handle.request() != nullptr);
}

BOOST_AUTO_TEST_CASE(visit)
{
	asio::io_context io;

	mock_handler<mock_request, mock_response> handle;
	flex_buffer buffer;
	error_code ec{};

	auto future = asio::co_spawn(
		io, [&]() -> asio::awaitable<flex_buffer> { co_return co_await handle.visit(buffer, 0, ec); },
		asio::use_future);

	io.run();

	auto buf = future.get();

	BOOST_TEST(std::string((char*)buf.data().data(), buf.size()) == std::string(mock_response::resp));
}

BOOST_AUTO_TEST_SUITE_END()