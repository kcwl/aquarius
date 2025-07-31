#pragma once
#include <aquarius/error_code.hpp>
#include <map>
#include <string_view>

using namespace std::string_view_literals;

namespace srdh
{
	enum class errc
	{
		success,
		no_return,
		invalid_cmd
	};

	inline std::string_view db_result_map(int result)
	{
		static std::map<int, std::string_view> db_results{
			{ static_cast<int>(errc::success), "success"sv },
			{ static_cast<int>(errc::no_return), "no operator for return back"sv },
			{ static_cast<int>(errc::invalid_cmd), "invalid command"sv }
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
			return "serviced errc";
		}

		[[nodiscard]] std::string message(int err_code) const override
		{
			return std::string(db_result_map(err_code).data());
		}
	};

	aquarius::error_code make_error_code(errc result)
	{
		return aquarius::error_code(std::error_code(static_cast<int>(result), db_result_category()));
	}
} // namespace serviced

namespace std
{
	template <>
	struct is_error_code_enum<srdh::errc> : std::true_type
	{};
} // namespace std