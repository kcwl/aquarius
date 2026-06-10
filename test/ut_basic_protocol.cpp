#define BOOST_TEST_NO_MAIN
#include <aquarius/basic_protocol.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_protocol)

class mock_header
{
public:
	int uuid;
};

class mock_body
{
public:
	int value;
};

class mock_protocol : public basic_protocol<mock_header, mock_body>
{
public:
	virtual error_code commit(flex_buffer&) override
	{
		return error_code{};
	}

	virtual error_code consume(flex_buffer&) override
	{
		return error_code{};
	}
};

BOOST_AUTO_TEST_CASE(protocol_ctor)
{
	mock_protocol p{};
	BOOST_TEST(p.header().uuid == mock_header{}.uuid);
	BOOST_TEST(p.body().value == mock_body{}.value);
}

BOOST_AUTO_TEST_CASE(protocol_copy_ctor)
{
	mock_protocol p{};
	p.header().uuid = 1;
	p.body().value = 1;

	mock_protocol p1(p);

	BOOST_TEST(p.header().uuid == p1.header().uuid);
	BOOST_TEST(p.body().value == p1.body().value);
}

BOOST_AUTO_TEST_CASE(protocol_copy_assign_ctor)
{
	mock_protocol p{};
	p.header().uuid = 1;
	p.body().value = 1;

	mock_protocol p1;
	p1 = p;

	BOOST_TEST(p.header().uuid == p1.header().uuid);
	BOOST_TEST(p.body().value == p1.body().value);
}

BOOST_AUTO_TEST_CASE(protocol_move_ctor)
{
	mock_protocol p{};
	p.header().uuid = 1;
	p.body().value = 1;

	mock_protocol p1(std::move(p));

	BOOST_TEST(p.header().uuid == 0);
	BOOST_TEST(p.body().value == 0);

	BOOST_TEST(p1.header().uuid == 1);
	BOOST_TEST(p1.body().value == 1);
}

BOOST_AUTO_TEST_CASE(protocol_move_assign_ctor)
{
	mock_protocol p{};
	p.header().uuid = 1;
	p.body().value = 1;

	mock_protocol p1;
	p1 = std::move(p);

	BOOST_TEST(p.header().uuid == 0);
	BOOST_TEST(p.body().value == 0);

	BOOST_TEST(p1.header().uuid == 1);
	BOOST_TEST(p1.body().value == 1);
}

BOOST_AUTO_TEST_SUITE_END()
