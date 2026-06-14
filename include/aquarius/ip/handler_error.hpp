#pragma once
#include <aquarius/error_code.hpp>
#include <boost/system/error_category.hpp>
#include <map>
#include <string>

namespace aquarius
{
	enum class handle_error
	{
		success = 0,
		not_exist
	};

	inline std::string handle_error_to_string(handle_error e)
	{
		static const std::map<handle_error, std::string> desc = {
			{ handle_error::success, "success" },
			{ handle_error::not_exist, "handler not exist" },
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
			return "handler error capagory";
		}

		[[nodiscard]] std::string message(int err_code) const override
		{
			return handle_error_to_string(static_cast<handle_error>(err_code));
		}
	};

	static proto_error_category& get_proto_error_category()
	{
		static proto_error_category category;
		return category;
	}

	inline error_code make_error_code(handle_error ec)
	{
		return error_code(static_cast<int>(ec), get_proto_error_category());
	}

} // namespace aquarius

namespace std
{
	template <>
	struct is_error_code_enum<aquarius::handle_error> : std::true_type
	{};
} // namespace std