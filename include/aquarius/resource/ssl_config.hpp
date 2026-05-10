#pragma once
#include <string>

namespace aquarius
{
	struct ssl_config
	{
		std::string crt;
		std::string key;
		std::string pem;
	};

	inline static ssl_config& create_ssl()
	{
		static ssl_config ssl;
		return ssl;
	}
} // namespace aquarius

#define SSL_CONFIG_INVOKE(cfg) CONFIG_MICRO(aquarius::ssl_config, cfg)