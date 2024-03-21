#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(basic_context)


struct person_body_request
{
	bool sex;
	std::vector<uint8_t> role_data;
	double mana;
	float hp;
	int32_t age;
	int64_t money;
	std::string name;
	uint32_t back_money;
	uint64_t crc;

private:
	friend class elastic::access;

	template<typename _Archive>
	void serialize(_Archive& ar)
	{
		ar& sex;
		ar& role_data;
		ar& mana;
		ar& hp;
		ar& age;
		ar& money;
		ar& name;
		ar& back_money;
		ar& crc;
	}
};

struct person_body_response
{
	bool sex;
	std::vector<uint8_t> role_data;
	double mana;
	float hp;
	int32_t age;
	int64_t money;
	std::string name;
	uint32_t back_money;
	uint64_t crc;

private:
	friend class elastic::access;

	template<typename _Archive>
	void serialize(_Archive& ar)
	{
		ar& sex;
		ar& role_data;
		ar& mana;
		ar& hp;
		ar& age;
		ar& money;
		ar& name;
		ar& back_money;
		ar& crc;
	}
};

using person_request = aquarius::request<person_body_request, 10000>;
using person_response = aquarius::response<person_body_response, 10001>;

class ctx_test_server : public aquarius::context<person_request, person_response>
{
public:
	ctx_test_server()
		: aquarius::context<person_request, person_response>("ctx_test_server")
	{}

public:
	virtual aquarius::error_code handle() override
	{
		std::cout << "server ctx\n";
		response_.body().age = 1;
		response_.body().name = "hello";

		send_response(0);

		return aquarius::error_code{};
	}
};

BOOST_AUTO_TEST_CASE(basic_message_context)
{
	auto ctx = std::make_shared<aquarius::basic_context>();

	auto req = std::make_shared<aquarius::basic_message>();

	aquarius::flex_buffer_t buffer{};

	aquarius::error_code ec;

	req->accept(buffer, ctx, nullptr, ec);

	BOOST_CHECK(ec);
}

BOOST_AUTO_TEST_CASE(call_back)
{
	{
		using connect_t = aquarius::connect<aquarius::tcp, aquarius::conn_mode::server, aquarius::ssl_mode::ssl>;

		aquarius::asio::io_service io_service;

		aquarius::asio::ip::tcp::socket socket(io_service);

		using ssl_context_t = aquarius::asio::ssl_context_t;

		ssl_context_t ssl_context(ssl_context_t::sslv23);

		ssl_context.set_options(ssl_context_t::default_workarounds | ssl_context_t::no_sslv2 |
			ssl_context_t::single_dh_use);

		ssl_context.use_certificate_chain_file("crt/server.crt");
		ssl_context.use_private_key_file("crt/server.key", ssl_context_t::pem);
		ssl_context.use_tmp_dh_file("crt/dh512.pem");

		

		auto conn = std::make_shared<connect_t>(std::move(socket), ssl_context);

		auto session_ptr = std::make_shared<aquarius::session<connect_t>>(conn);

		aquarius::invoke_session_helper::push(session_ptr);

		session_ptr->attach(1300, std::make_shared<aquarius::basic_context>());
		session_ptr->attach(1300, std::make_shared<aquarius::basic_context>());
		session_ptr->attach(1301, std::make_shared<ctx_test_server>());
	}
}

BOOST_AUTO_TEST_CASE(function)
{
	auto request_ptr = std::make_shared<person_request>();

	aquarius::error_code ec{};

	aquarius::flex_buffer_t buffer{};

	request_ptr->to_binary(buffer, ec);

	auto context_ptr = std::dynamic_pointer_cast<aquarius::basic_context>(std::make_shared<ctx_test_server>());

	BOOST_CHECK(!request_ptr->accept(buffer, context_ptr, nullptr,ec));
}

BOOST_AUTO_TEST_SUITE_END()