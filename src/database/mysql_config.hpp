#pragma once
#include <aquarius/singleton.hpp>
#include <string>

namespace aquarius
{
	namespace db
	{
		struct mysql_config : singleton<mysql_config>
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
			return mysql_config::get_mutable_instance();
		}
	} // namespace db
} // namespace aquarius