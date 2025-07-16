#pragma once
#include <aquarius/error_code.hpp>
#include <map>
#include <string_view>

using namespace std::string_view_literals;

namespace db
{
	enum class db_result
	{
		success,
		invalid_account,
		no_return,
	};

	inline std::string_view db_result_map(int result)
	{
		static std::map<int, std::string_view> db_results{
			{ static_cast<int>(db_result::success), "success"sv },
			{ static_cast<int>(db_result::invalid_account), "invalid account"sv },
			{ static_cast<int>(db_result::no_return), "no operator for return back"sv }
		};

		return db_results[result];
	}

	class db_result_category : public std::error_category
	{
	public:
		constexpr db_result_category() noexcept
			: std::error_category(_Generic_addr)
		{}

		[[nodiscard]] const char* name() const noexcept override
		{
			return "db result";
		}

		[[nodiscard]] std::string message(int err_code) const override
		{
			return std::string(db_result_map(err_code).data());
		}
	};

	aquarius::error_code make_error_code(db_result result)
	{
		return aquarius::error_code(std::error_code(static_cast<int>(result), db_result_category()));
	}
} // namespace db

namespace std
{
	template<>
	struct is_error_code_enum<db::db_result> : std::true_type {};
}