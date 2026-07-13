#pragma once
#pragma once
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
		struct permission
		{
			int64_t id;
			int64_t role_id;
			int64_t perm_id;
		};

		struct perm_desc
		{
			int64_t id;
			std::string perm;
		};

		AQUARIUS_MODULE(permission_module)
		{
		public:
			virtual auto run() -> asio::awaitable<bool> override;

		public:
			auto add_desc(const std::vector<std::string>& perms) -> asio::awaitable<bool>;

			auto add_role(int64_t role_id, const std::vector<int64_t>& perms) -> asio::awaitable<bool>;

			auto remove(int64_t id) -> asio::awaitable<bool>;

			auto remove_role(int64_t role_id) -> asio::awaitable<bool>;

			auto update(int64_t id, const std::string& perm) -> asio::awaitable<bool>;

			auto view(int64_t id) const -> asio::awaitable<std::string>;

			auto check(int64_t role_id, int64_t perm) -> asio::awaitable<bool>;

		private:
			auto load_permission() -> asio::awaitable<void>;

			auto load_permission_desc() -> asio::awaitable<void>;

			void fill_permissions(const permission& perms);

			void fill_permissions_desc(const perm_desc& perms);

		private:
			std::map<int64_t, std::vector<std::shared_ptr<permission>>> perms_;

			std::map<int64_t, std::string> perm_descs_;

			int64_t index_;

			int64_t perm_index_;
		};
	} // namespace login
} // namespace aquarius