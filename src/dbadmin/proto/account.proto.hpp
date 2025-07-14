#pragma once
#include <aquarius_protocol.hpp>


struct query_account_req
{
	std::string account;
};
struct query_account_resp
{
	std::string passwd;
};struct rpc_query_account
{
	constexpr static auto id = 100;
	using request = aquarius::tcp_request<query_account_req>;
	using response = aquarius::tcp_response<query_account_resp>;
};

