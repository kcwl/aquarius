#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/cmd_options.hpp>
#include <string>

BOOST_AUTO_TEST_SUITE(cmd)

BOOST_AUTO_TEST_CASE(command_argv)
{
	aquarius::cmd_options cmd{ "test_command" };

	char* argv[] = { (char*)"test.exe", (char*)"--hello", (char*)"x",(char*)"--world", (char*)"24"};

	cmd.add_option<std::string>("hello", "test first param");
	cmd.add_option<int>("world", "test second param");

	cmd.load_options(5, argv);

	BOOST_CHECK_EQUAL(cmd.option<std::string>("hello"), "x");
	BOOST_CHECK_EQUAL(cmd.option<int>("world"), 24);

	BOOST_CHECK_EQUAL(cmd.desc(), "test_command");

	auto ops = cmd.options();

	BOOST_CHECK_EQUAL(ops.size(), 2);

	BOOST_CHECK_EQUAL(ops.front()->description(), "test first param");
	BOOST_CHECK_EQUAL(ops.back()->description(), "test second param");
}

BOOST_AUTO_TEST_CASE(command_argv_vector)
{
	aquarius::cmd_options cmd{ "test_command" };

	std::vector<std::string> argvs = { "test.exe", "--hello","x","--world","24"};

	cmd.add_option<std::string>("hello", "test first param");
	cmd.add_option<int>("world", "test second param");

	cmd.load_options(argvs);

	BOOST_CHECK_EQUAL(cmd.option<std::string>("hello"), "x");
	BOOST_CHECK_EQUAL(cmd.option<int>("world"), 24);

	BOOST_CHECK_EQUAL(cmd.desc(), "test_command");

	auto ops = cmd.options();

	BOOST_CHECK_EQUAL(ops.size(), 2);

	BOOST_CHECK_EQUAL(ops.front()->description(), "test first param");
	BOOST_CHECK_EQUAL(ops.back()->description(), "test second param");
}


BOOST_AUTO_TEST_SUITE_END()