#pragma once
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
        struct account
        {
            std::size_t id;
            std::string acc;
            std::string password;
        };


        AQUARIUS_MODULE(auth_module)
        {
        public:
            auth_module() = default;

        public:
            virtual auto run() ->asio::awaitable<bool> override;

        public:
            auto create_account(const std::string & username, const std::string & password) -> asio::awaitable<error_code>;

            auto remove_account(const std::string & username) -> asio::awaitable<error_code>;

            auto change_password(const std::string & username, const std::string & old_passwd, const std::string & new_passwd, const std::string & new_passwd_sec) -> asio::awaitable<error_code>;

            auto check_password(const std::string & username, const std::string & password) -> asio::awaitable<error_code>;

        private:
            auto load_account() -> asio::awaitable<void>;

            void fill_account(const account & acc);

            auto sql_insert(const account & acc) -> asio::awaitable<error_code>;

            auto sql_remove(const account & acc) -> asio::awaitable<error_code>;

            auto sql_update(const account & acc) -> asio::awaitable<std::size_t>;

        private:
            std::map<std::string, std::shared_ptr<account>> accounts_;
        };
	}
}