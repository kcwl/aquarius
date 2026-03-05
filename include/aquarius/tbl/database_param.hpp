#pragma once
#include <string>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	struct database_param
	{
		constexpr static auto path = "mysql"sv;

		bool enable()
		{
			return true;
		}

		std::string host;
		std::string user;
		std::string password;
		int port;
		std::string db;
		int32_t pool_size = 1;
		std::string CA;
		int32_t timeout;
		int reconnect;
		bool enable_transaction;
	};
} // namespace aquarius