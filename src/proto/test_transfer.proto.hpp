#pragma once
#include <aquarius_protocol.hpp>


struct test_transfer_req
{
	std::string test_message;
};
struct test_transfer_resp
{
};struct rpc_test_transfer
{
	constexpr static auto id = 1100;
	using request = aquarius::tcp_request<test_transfer_req>;
	using response = aquarius::tcp_response<test_transfer_resp>;
};

