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
		constexpr static std::string_view id = "1"sv;
		using request = transfer_flow_req;
		using response = transfer_flow_resp;
	};
} // namespace aquarius