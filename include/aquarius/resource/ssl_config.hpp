#pragma once
#include <aquarius/resource/config_tag_invoke.hpp>
#include <string>

namespace aquarius
{
	struct ssl_config
	{
		std::string crt;
		std::string key;
		std::string pem;
	};
} // namespace aquarius

#define SSL_CONFIG_INVOKE(cfg) CONFIG_MICRO(aquarius::ssl_config, cfg)