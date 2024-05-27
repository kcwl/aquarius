#pragma once
#include <map>
#include <string>
#include <system_error>

namespace aquarius
{
	enum class error_code_result
	{
		ok,			// success complete
		pending,	// waiting for handle
		incomplete, // package is not complete
		unknown,	// unknown protocol
		nosession   // non session
	};

	inline std::map<int, std::string> error_map()
	{
		static const std::map<int, std::string> error_arr = {
			{ static_cast<int>(error_code_result::ok), "successful" },
			{ static_cast<int>(error_code_result::pending), "wait for handle pending" },
			{ static_cast<int>(error_code_result::incomplete), "package is not complete" },
			{ static_cast<int>(error_code_result::unknown), "unknown protocol" }
		};

		return error_arr;
	}

	class basic_error_category : public std::error_category
	{
	public:
		virtual const char* name() const noexcept override
		{
			return "aquarius basic error category";
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
		static basic_error_category category;

		return category;
	}

	class error_code : public std::error_code
	{
	public:
		error_code()
			: error_code(0)
		{}

		error_code(int ec)
			: std::error_code(ec, get_error_category())
		{}

		error_code(error_code_result ec)
			: error_code(static_cast<int>(ec))
		{

		}

	public:
		error_code& operator=(error_code_result ec)
		{
			this->assign(static_cast<int>(ec), get_error_category());

			return *this;
		}

	public:
		error_code(const error_code&) = default;
		error_code& operator=(const error_code&) = default;

		error_code(error_code&&) = default;
		error_code& operator=(error_code&&) = default;
	};
} // namespace aquarius