#define BOOST_TEST_NO_MAIN
#include <aquarius/module/module_router.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;
using namespace std::chrono_literals;

class mock_module : public basic_module<mock_module>
{
public:
	bool say()
	{
		return true;
	}
};

class mock_no_init_module : public basic_module<mock_no_init_module>
{
public:
	virtual bool init() override
	{
		return false;
	}
};

class mock_direct_base_module : public module_base
{
public:
	virtual std::string name() const override
	{
		return "mock base";
	};

	virtual bool init() override
	{
		return true;
	}

	virtual void stop() override
	{}

	virtual bool enable() override
	{
		return true;
	}

	virtual void timer(std::chrono::milliseconds) override
	{}

	virtual auto run() -> asio::awaitable<bool> override
	{
		co_return true;
	}
};

BOOST_AUTO_TEST_SUITE(ut_module_router)

BOOST_AUTO_TEST_CASE(regist)
{
	module_router router;
	auto result = router.regist<mock_module>();

	BOOST_TEST(result);
}

BOOST_AUTO_TEST_CASE(multi_regist)
{
	module_router router;
	auto result = router.regist<mock_module>();

	BOOST_TEST(result);

	result = router.regist<mock_module>();

	BOOST_TEST(!result);
}

BOOST_AUTO_TEST_CASE(router_run)
{
	module_router router;
	router.regist<mock_module>();
	router.regist<mock_no_init_module>();

	io_service_pool pool(1);

	BOOST_CHECK_NO_THROW(router.run(pool));
}
BOOST_AUTO_TEST_CASE(router_timer)
{
	module_router router;
	router.regist<mock_module>();

	BOOST_CHECK_NO_THROW(router.timer(1s));
}

BOOST_AUTO_TEST_CASE(schedules)
{
	module_router router;
	router.regist<mock_module>();

	auto f = [](mock_module* m) { return m->say(); };

	BOOST_TEST((router.schedule<mock_module, bool>(f)));

	router.regist<mock_direct_base_module>();

	auto f1 = [](mock_direct_base_module*) { return true; };

	BOOST_TEST((!router.schedule<mock_direct_base_module, bool>(f1)));
}

BOOST_AUTO_TEST_CASE(async_schedules)
{
	module_router router;
	router.regist<mock_module>();

	asio::io_context io{};

	auto future = asio::co_spawn(
		io,
		[&]() -> asio::awaitable<void>
		{
			auto f = [&](mock_module* m) -> asio::awaitable<bool> { co_return m->say(); };

			BOOST_TEST(co_await (router.async_schedule<mock_module, bool>(f)));

			router.regist<mock_direct_base_module>();

			auto f1 = [](mock_direct_base_module* m) -> asio::awaitable<bool> { co_return true; };

			BOOST_TEST(!co_await (router.async_schedule<mock_direct_base_module, bool>(f1)));
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(no_init_module)
{}

BOOST_AUTO_TEST_SUITE_END()