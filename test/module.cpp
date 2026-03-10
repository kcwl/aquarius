#define BOOST_TEST_NO_MAIN
#include <aquarius/module/schedule.hpp>
#include <boost/test/unit_test.hpp>

using namespace std::chrono_literals;

class test_module : public aquarius::_module<test_module>
{
	using base = aquarius::_module<test_module>;

public:
	test_module(const std::string& name)
		: base(name)
		, test_increament_(0)
	{}

public:
	void increament(int& v)
	{
		v++;
	}

	virtual void timer(std::chrono::milliseconds)
	{
		test_increament_++;
	}

	int get_test_increament() const
	{
		return test_increament_;
	}

private:
	int test_increament_;
};

// AQUARIUS_MODULE(test_module)

BOOST_AUTO_TEST_SUITE(test_module_suite)

BOOST_AUTO_TEST_CASE(ctor)
{
	aquarius::io_context io{};

	test_module tm("test_module");

	BOOST_TEST(tm.name() == "test_module");
}

BOOST_AUTO_TEST_CASE(test_module_virsual_func)
{
	test_module tm("test_module");

	BOOST_TEST(tm.config());

	BOOST_TEST(tm.init());

	BOOST_TEST(tm.enable());

	tm.timer(10ms);

	BOOST_CHECK(tm.get_test_increament() == 1);

	tm.stop();

	BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(test_async_module)
{
	aquarius::io_context io{};

	test_module tm("test_module");

	int increament = 0;

	auto f = [&](test_module* ptr) mutable -> aquarius::awaitable<void> { co_return ptr->increament(increament); };

	aquarius::co_spawn(io, tm.visit(std::make_shared<aquarius::module_data<void, decltype(f)>>(f)),
					   aquarius::detached);

	io.run();

	BOOST_TEST(increament == 1);
}

BOOST_AUTO_TEST_CASE(test_sync_module)
{
	test_module tm("test_module");

	int increament = 0;

	auto f = [&](test_module* ptr) { return ptr->increament(increament); };

	tm.visit_sync(std::make_shared<aquarius::module_sync_data<void, decltype(f)>>(f));

	BOOST_TEST(increament == 1);
}

BOOST_AUTO_TEST_SUITE_END()