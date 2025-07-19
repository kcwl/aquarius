#pragma once
#include <aquarius_protocol.hpp>


struct cmd_req
{
	std::string input;
};
struct cmd_resp
{
	std::string output;
};struct rpc_cmd
{
	constexpr static auto id = 201;
	using request = aquarius::tcp_request<cmd_req>;
	using response = aquarius::tcp_response<cmd_resp>;
};

