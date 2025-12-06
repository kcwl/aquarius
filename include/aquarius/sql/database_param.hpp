#pragma once
#include <optional>
#include <string>

namespace aquarius
{
	struct database_param
	{
		std::string host;
		std::string user;
		std::string password;
		int port;
		std::string db;
		int32_t pool_size = 1;
		std::string CA;
		std::optional<int32_t> timeout;
		std::optional<bool> reconnect;
		std::optional<bool> enable_transaction;
	};
} // namespace aquarius