#include <aquarius/module/module_router.hpp>
#include <aquarius/module/schedule.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_schedule)

struct TestModule : public module<TestModule>
{
	auto foo() -> asio::awaitable<int>
	{
		co_return 5;
	}
};

BOOST_AUTO_TEST_CASE(mpc_call_member_traits)
{
	aquarius::module_router::get_mutable_instance().put<TestModule>(0);

	using mp = aquarius::member_func_pointer<decltype(&TestModule::foo)>;
	static_assert(std::is_same_v<mp::return_type, asio::awaitable<int>>);
	static_assert(std::is_same_v<mp::class_type, TestModule>);

	BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(mpc_async_call_module)
{
	io_service_pool pool(2);

	auto future = asio::co_spawn(
		pool.get_io_service(),
		[&]() -> asio::awaitable<int>
		{
			co_await aquarius::module_router::get_mutable_instance().run(pool);

			co_return co_await mpc_async_call<&TestModule::foo>();
		},
		asio::use_future);

	std::thread t([&] { pool.run(); });

	BOOST_TEST(future.get() == 5);

	pool.stop();

	t.join();
}

BOOST_AUTO_TEST_SUITE_END()
