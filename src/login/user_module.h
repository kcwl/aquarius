#pragma once
#include "user.h"
#include <aquarius/basic_module.hpp>
#include <aquarius/module/module_register.hpp>
#include <map>
#include <memory>

namespace aquarius
{
	namespace login
	{

		AQUARIUS_MODULE_PRI(user_module, 1)
		{
		public:
			user_module() = default;

		public:
			auto insert(std::shared_ptr<user> user_ptr) -> asio::awaitable<void>;

			auto erase(std::size_t id) -> asio::awaitable<void>;

			auto check_permission(std::size_t user_id, int64_t perm) -> asio::awaitable<bool>;

			auto create_permission(std::size_t user_id, const std::vector<int64_t>& perms) -> asio::awaitable<void>;

			auto remove_permission(std::size_t user_id, int64_t id) -> asio::awaitable<void>;

			auto view_permission(std::size_t user_id) -> asio::awaitable<std::vector<std::string>>;

		private:
			auto load_role_permission() -> asio::awaitable<void>;

		private:
			std::map<std::size_t, std::shared_ptr<user>> users_;
		};
	} // namespace login
} // namespace aquarius