#include <aquarius/virgo/basic_http_protocol.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_http_protocol)

struct mock_header
{
	error_code serialize(flex_buffer&)
	{
		return error_code{};
	}

	error_code deserialize(flex_buffer&)
	{
		uuid = 1;
		return error_code{};
	}

	void content_length(std::size_t) {};

	int uuid;
};

struct mock_body
{
	error_code serialize(flex_buffer&)
	{
		return error_code{};
	}

	error_code deserialize(flex_buffer&)
	{
		value = 2;
		return error_code{};
	}

	std::size_t byte_size()
	{
		return 1;
	}

	int value;
};

struct mock_failed_header
{
	error_code serialize(flex_buffer&)
	{
		throw std::runtime_error("failed header serialize");
	}

	error_code deserialize(flex_buffer&)
	{
		throw std::runtime_error("failed header deserialize");
	}

	int uuid;
};

struct mock_failed_body
{
	error_code serialize(flex_buffer&)
	{
		throw std::runtime_error("failed body serialize");;
	}

	error_code deserialize(flex_buffer&)
	{
		throw std::runtime_error("failed body deserialize");;
	}

	std::size_t byte_size()
	{
		return 1;
	}

	int value;
};

BOOST_AUTO_TEST_CASE(ctor)
{
	basic_http_protocol<true, "", mock_body> proto{};

	BOOST_TEST(proto.method() == http_method::post);
}

BOOST_AUTO_TEST_CASE(move_ctor)
{
	basic_http_protocol<true, "", mock_body> proto{};
	proto.method(http_method::get);

	auto proto1(std::move(proto));
	BOOST_TEST(proto1.method() == http_method::get);

	BOOST_TEST(proto.method() == http_method::post);
}

BOOST_AUTO_TEST_CASE(copy_ctor)
{
	basic_http_protocol<true, "", mock_body> proto{};
	proto.method(http_method::get);

	auto proto1(proto);
	BOOST_TEST(proto1.method() == http_method::get);

	BOOST_TEST(proto.method() == http_method::get);
}

BOOST_AUTO_TEST_CASE(move_copy_ctor)
{
	basic_http_protocol<true, "", mock_body> proto{};
	proto.method(http_method::get);

	auto proto1 = std::move(proto);
	BOOST_TEST(proto1.method() == http_method::get);

	BOOST_TEST(proto.method() == http_method::post);
}

BOOST_AUTO_TEST_CASE(assign)
{
	basic_http_protocol<true, "", mock_body> proto{};
	proto.method(http_method::get);

	basic_http_protocol<true, "", mock_body> proto1;
	proto1 = proto;
	BOOST_TEST(proto1.method() == http_method::get);

	BOOST_TEST(proto.method() == http_method::get);
}

BOOST_AUTO_TEST_CASE(move_assign)
{
	basic_http_protocol<true, "", mock_body> proto{};
	proto.method(http_method::get);

	basic_http_protocol<true, "", mock_body> proto1;
	proto1 = std::move(proto);
	BOOST_TEST(proto1.method() == http_method::get);

	BOOST_TEST(proto.method() == http_method::post);
}

BOOST_AUTO_TEST_CASE(http_commit_failed)
{
	basic_http_protocol<true, "", mock_failed_body> proto{};

	flex_buffer buffer{};
	auto ec = proto.commit(buffer);

	BOOST_TEST(ec == ip::error::commit);
}

BOOST_AUTO_TEST_CASE(http_commit_success)
{
	basic_http_protocol<true, "", mock_body> proto{};

	flex_buffer buffer{};
	auto ec = proto.commit(buffer);

	BOOST_TEST(!ec);
}

BOOST_AUTO_TEST_CASE(http_consume_failed)
{
	basic_http_protocol<true, "", mock_failed_body> proto{};

	flex_buffer buffer{};
	auto ec = proto.consume(buffer);

	BOOST_TEST(ec == ip::error::consume);
}

BOOST_AUTO_TEST_CASE(http_consume_success)
{
	basic_http_protocol<true, "", mock_body> proto{};

	flex_buffer buffer{};
	auto ec = proto.consume(buffer);

	BOOST_TEST(!ec);
}

BOOST_AUTO_TEST_SUITE_END()
