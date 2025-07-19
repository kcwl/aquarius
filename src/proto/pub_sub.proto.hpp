#pragma once
#include <aquarius_protocol.hpp>


struct channel_publish_req
{
	uint32_t topic;
	std::vector<uint8_t> channel_data;
};
struct channel_publish_resp
{
};
struct channel_subscribe_req
{
	uint32_t topic;
};
struct channel_subscribe_resp
{
};
struct channel_remove_req
{
	std::vector<uint32_t> topics;
};
struct channel_remove_resp
{
};struct rpc_channel_publish
{
	constexpr static auto id = 990;
	using request = aquarius::tcp_request<channel_publish_req>;
	using response = aquarius::tcp_response<channel_publish_resp>;
};

struct rpc_channel_subscribe
{
	constexpr static auto id = 991;
	using request = aquarius::tcp_request<channel_subscribe_req>;
	using response = aquarius::tcp_response<channel_subscribe_resp>;
};

struct rpc_channel_remove
{
	constexpr static auto id = 992;
	using request = aquarius::tcp_request<channel_remove_req>;
	using response = aquarius::tcp_response<channel_remove_resp>;
};

