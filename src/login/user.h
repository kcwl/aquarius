#pragma once
#include <aquarius.hpp>
#include <string>
#include <vector>

namespace aquarius
{
	namespace login
	{
		struct role_permission
		{
			int64_t id;
			int64_t user_id;
			int64_t role_id;
		};

		class user
		{
		public:
			auto check_permission(int64_t perm) -> asio::awaitable<bool>;

			void create_permission(const std::vector<int64_t>& perms);

			void remove_permission(int64_t role_id);

			auto view_permission() -> asio::awaitable<std::vector<std::string>>;

			std::size_t id() const;

			auto load_role_permission() -> asio::awaitable<void>;

		private:
			void fill_role_permission(const role_permission& role_perm);

		private:
			std::size_t id_;
			std::string account_;
			std::vector<int64_t> role_ids_;
		};
	} // namespace login
} // namespace aquarius