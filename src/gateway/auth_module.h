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
			auth_module() = default;

		public:
			bool auth(const std::string& username, const std::string& password);
		};
	} // namespace gateway
} // namespace aquarius