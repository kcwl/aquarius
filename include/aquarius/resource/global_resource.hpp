#pragma once
#include <aquarius/singleton.hpp>
#include <string>

namespace aquarius
{
	class global_resource : public singleton<global_resource>
	{
	public:
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

	public:
		mysql_config& mysql()
		{
			return mysql_config_;
		}

	private:
		mysql_config mysql_config_;
	};
} // namespace aquarius