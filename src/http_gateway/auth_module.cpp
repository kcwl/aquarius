#include "auth_module.h"
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	namespace gateway
	{
		auto auth_module::auth(const std::string& username, const std::string& password) -> asio::awaitable<bool>
		{
			co_return true;
		}
	} // namespace gateway
} // namespace aquarius