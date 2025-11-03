#pragma once
#include <string_view>
#include <map>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	enum class serialize_error
	{
		success,
		not_enough_space,
		overflow
	};

	inline std::string_view from_error_string(serialize_error e)
	{
		static const std::map<serialize_error, std::string_view> desc = { { serialize_error::success, "success" },
																		  { serialize_error::overflow, "overflow" },
																		  { serialize_error::not_enough_space,
																			"no enough space" } };

		return desc.at(e);
	}

	class serialize_error_category : public boost::system::error_category
	{
	public:
		constexpr serialize_error_category() = default;

		[[nodiscard]] const char* name() const noexcept override
		{
			return "serialize error category";
		}

		[[nodiscard]] std::string message(int err_code) const override
		{
			return std::string(from_error_string(static_cast<serialize_error>(err_code)).data());
		}
	};

	static serialize_error_category& get_serialize_error_category()
	{
		static serialize_error_category category{};

		return category;
	}

	inline error_code make_error_code(serialize_error ec)
	{
		return error_code(static_cast<int>(ec), get_serialize_error_category());
	}
} // namespace aquarius

namespace std
{
	template <>
	struct is_error_code_enum<aquarius::serialize_error> : std::true_type
	{};
} // namespace std