#pragma once
#include <string>

namespace aquarius
{
	struct mysql_config
	{
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

	inline static mysql_config& create_mysql()
	{
		static mysql_config mysql;
		return mysql;
	}
} // namespace aquarius

#define MYSQL_CONFIG_INVOKE(cfg) CONFIG_MICRO(aquarius::mysql_config, cfg)