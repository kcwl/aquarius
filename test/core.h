#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(core)

BOOST_AUTO_TEST_CASE(error_code)
{
	{
		aquarius::error_code ec(-1);

		BOOST_CHECK(ec.message() == "unknown error");
	}

	{
		aquarius::error_code ec(0);

		BOOST_CHECK(std::string(ec.category_name()).compare("aquarius basic error category") == 0);
	}
}

BOOST_AUTO_TEST_CASE(async_pool)
{
	try
	{
		aquarius::post([]
					   {
						   std::exception_ptr exception{};


						   throw exception;
					   });

		BOOST_CHECK(true);
	}
	catch (...)
	{
		BOOST_CHECK(false);
	}
}

BOOST_AUTO_TEST_SUITE_END()