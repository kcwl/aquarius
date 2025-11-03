#pragma once
#include <string_view>
#include <map>
#include <boost/system/error_category.hpp>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	enum class tcp_error
	{
		success,
		no_space
	};

	inline std::string_view from_tcp_error_string(tcp_error e)
	{
		static const std::map<tcp_error, std::string_view> desc = {
			{tcp_error::success, "success"},
			{tcp_error::no_space, "no space"}
		};

		return desc.at(e);
	}

	class tcp_error_category : public boost::system::error_category
	{
	public:
		constexpr tcp_error_category() = default;

		[[nodiscard]] const char* name() const noexcept override
		{
			return "serialize error category";
		}

		[[nodiscard]] std::string message(int err_code) const override
		{
			return std::string(from_tcp_error_string(static_cast<tcp_error>(err_code)).data());
		}
	};

	static tcp_error_category& get_tcp_error_category()
	{
		static tcp_error_category category{};

		return category;
	}

	inline error_code make_error_code(tcp_error ec)
	{
		return error_code(static_cast<int>(ec), get_tcp_error_category());
	}
}

namespace std
{
	template <>
	struct is_error_code_enum<aquarius::tcp_error> : std::true_type
	{
	};
} // namespace std