#pragma once
#include <aquarius/module/module_register.hpp>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_MODULE(auth_module)
		{
		public:
			auth_module() = default;

		public:
			auto auth(const std::string& username, const std::string& password) -> asio::awaitable<bool>;
		};
	} // namespace gateway
} // namespace aquarius