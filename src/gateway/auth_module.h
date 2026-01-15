#pragma once
#include <aquarius/module/module.hpp>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		class auth_module : public no_config_module<auth_module>
		{
		public:
			auth_module(const std::string& name);

		public:
			auto auth(const std::string& username, const std::string& password) ->awaitable<bool>;
		};
	} // namespace gateway
} // namespace aquarius