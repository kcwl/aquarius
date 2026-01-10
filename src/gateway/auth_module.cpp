#include "auth_module.h"
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	namespace gateway
	{
		auth_module::auth_module(const std::string& name)
			: no_config_module<auth_module>(name)
		{

		}

		bool auth_module::auth(const std::string& username, const std::string& password)
		{
			(void)username;
			(void)password;

			return true;
		}
	} // namespace gateway
} // namespace aquarius

AQUARIUS_MODULE_NAMESPACE(aquarius::gateway, auth_module)