#pragma once
#include <aquarius/error.hpp>
#include <map>
#include <string>
#include <boost/system/error_category.hpp>

namespace aquarius
{
	namespace detail
	{
		inline std::map<int, std::string> error_map()
		{
			static const std::map<int, std::string> error_arr = {
				{ static_cast<int>(package::ok), "successful" },
				{ static_cast<int>(package::pending), "wait for handle pending" },
				{ static_cast<int>(package::incomplete), "package is not complete" },
				{ static_cast<int>(package::unknown), "unknown protocol" },
				{ static_cast<int>(package::nosession), "session is not exist" },
				{ static_cast<int>(package::timeout), "context handle timeout" }
			};

			return error_arr;
		}

		class error_category : public std::error_category
		{
		public:
			virtual const char* name() const noexcept override
			{
				return "aquarius error category";
			}

			virtual std::string message(int ec) const override
			{
				const auto error = error_map();

				auto iter = error.find(ec);

				if (iter == error.end())
				{
					return "unknown error";
				}

				return iter->second;
			}
		};

		inline const auto& get_error_category() noexcept
		{
			static error_category category;

			return category;
		}
	} // namespace detail
} // namespace aquarius