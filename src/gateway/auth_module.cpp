#include "auth_module.h"

namespace aquarius
{
	namespace gateway
	{
		bool auth_op::auth(const std::string_view& username, const std::string_view& password)
		{
			(void)username;
			(void)password;

			return true;
		}
	}
}