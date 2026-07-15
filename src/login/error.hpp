#pragma once
#include <aquarius/error_code.hpp>
#include <map>
#include <string>

namespace aquarius
{
	namespace login
	{
		enum class error
		{
			success,
			not_exist_account,
			incorrect_passwd,
			not_exist_in_pool
		};

		inline std::string error_to_string(login::error e)
		{
			static const std::map<login::error, std::string> desc = {
				{ login::error::success, "success" },
				{ login::error::not_exist_account, "account is not exist" },
				{ login::error::incorrect_passwd, "incorrect password" },
				{ login::error::not_exist_in_pool, "not exist in pool" },
			};

			auto iter = desc.find(e);

			return iter == desc.end() ? "unknown error" : iter->second;
		}

		class proto_error_category : public boost::system::error_category
		{
		public:
			constexpr proto_error_category() = default;

			[[nodiscard]] const char* name() const noexcept override
			{
				return "proto_error_category";
			}

			[[nodiscard]] std::string message(int err_code) const override
			{
				return error_to_string(static_cast<error>(err_code));
			}
		};

		static proto_error_category& get_proto_error_category()
		{
			static proto_error_category category;
			return category;
		}

		inline error_code make_error_code(error ec)
		{
			return error_code(static_cast<int>(ec), get_proto_error_category());
		}
	} // namespace ip
} // namespace aquarius

namespace std
{
	template <>
	struct is_error_code_enum<aquarius::login::error> : std::true_type
	{};
} // namespace std