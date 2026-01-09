#include "auth_module.h"

namespace aquarius
{
	namespace gateway
	{
		bool auth_module::auth(const std::string& username, const std::string& password)
		{
			(void)username;
			(void)password;

			return true;
		}
	} // namespace gateway
} // namespace aquarius