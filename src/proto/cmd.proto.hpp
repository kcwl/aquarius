#pragma once
#include <aquarius_protocol.hpp>


struct help_req
{
};
struct help_resp
{
	std::string msg;
};
struct list_req
{
};
struct list_resp
{
	std::vector<std::string> msg;
};
struct add_req
{
	std::string input;
};
struct add_resp
{
};
struct remove_req
{
	std::string input;
};
struct remove_resp
{
};struct rpc_cmd_help
{
	constexpr static auto id = 201;
	using request = aquarius::tcp_request<help_req>;
	using response = aquarius::tcp_response<help_resp>;
};

struct rpc_cmd_list
{
	constexpr static auto id = 202;
	using request = aquarius::tcp_request<list_req>;
	using response = aquarius::tcp_response<help_resp>;
};

struct rpc_cmd_add
{
	constexpr static auto id = 203;
	using request = aquarius::tcp_request<add_req>;
	using response = aquarius::tcp_response<add_resp>;
};

struct rpc_cmd_remove
{
	constexpr static auto id = 204;
	using request = aquarius::tcp_request<remove_req>;
	using response = aquarius::tcp_response<remove_resp>;
};

