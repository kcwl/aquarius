#pragma once
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
		struct permisson
		{
			int64_t id;
			int64_t role_id;
			std::string permissions;
		};

		AQUARIUS_MODULE(permisson_module)
		{
		public:
			virtual auto run() -> asio::awaitable<bool> override;

		public:
			bool check(int64_t role_id, const std::string& perm);

			auto create(int64_t role_id, const std::vector<std::string>& perms) -> asio::awaitable<void>;

			auto update(int64_t id, const std::string& perm) -> asio::awaitable<bool>;

			auto remove(int64_t id) -> asio::awaitable<void>;

			std::vector<std::string> view(int64_t role_id) const;

		private:
			auto load_permission() -> asio::awaitable<void>;

			void fill_permissions(const permisson& perms);

			auto sql_insert(const std::vector<permisson>& perms) -> asio::awaitable<void>;
			auto sql_update(const permisson& p) -> asio::awaitable<std::size_t>;
			auto sql_remove(const permisson& p) -> asio::awaitable<void>;

		private:
			std::map<int64_t, std::vector<std::shared_ptr<permisson>>> perms_;

			std::map<int64_t, std::shared_ptr<permisson>> id_perms_;
		};
	} // namespace login
} // namespace aquarius