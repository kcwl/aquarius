#pragma once
#include <memory>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		struct auth_task
		{
			auth_task(const std::string& username, const std::string& password)
				: username(username)
				, password(password)
			{}

			template <typename Core>
			bool operator()(std::shared_ptr<Core> core_ptr)
			{
				return core_ptr->auth(username, password);
			}

			std::string username;
			std::string password;
		};
	} // namespace gateway
} // namespace aquarius