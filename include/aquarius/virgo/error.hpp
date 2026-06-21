#pragma once
#include <aquarius/error_code.hpp>
#include <map>
#include <string>

namespace aquarius
{
	namespace ip
	{
		enum class error
		{
			success,
			commit,
			consume
		};

		inline std::string error_to_string(ip::error e)
		{
			static const std::map<ip::error, std::string> desc = {
				{ ip::error::success, "success" },
				{ ip::error::commit, "commit error when parse proto" },
				{ ip::error::consume, "consume error when parse proto" },
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
	struct is_error_code_enum<aquarius::ip::error> : std::true_type
	{};
} // namespace std