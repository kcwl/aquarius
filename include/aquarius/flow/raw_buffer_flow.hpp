#pragma once
#include <memory>

namespace aquarius
{
	struct transfer_flow_req
	{
		std::vector<char> data;
	};

	struct transfer_flow_resp
	{
		std::vector<char> data;
	};

	struct rpc_transfer_flow
	{
		constexpr static std::size_t id = 1;
		using request = transfer_flow_req;
		using response = transfer_flow_resp;
	};
} // namespace aquarius