#pragma once
#include <map>
#include <string>
#include <system_error>

namespace aquarius
{
	enum class errc
	{
		ok,			// success complete
		pending,	// waiting for handle
		incomplete, // package is not complete
		unknown,	// unknown protocol
		nosession,  // non session
		timeout     // timeout
	};

	inline std::map<int, std::string> error_map()
	{
		static const std::map<int, std::string> error_arr = {
			{ static_cast<int>(errc::ok), "successful" },
			{ static_cast<int>(errc::pending), "wait for handle pending" },
			{ static_cast<int>(errc::incomplete), "package is not complete" },
			{ static_cast<int>(errc::unknown), "unknown protocol" },
			{static_cast<int>(errc::nosession), "session is not exist"},
			{static_cast<int>(errc::timeout), "context handle timeout"}
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

		error_code(errc ec)
			: error_code(static_cast<int>(ec))
		{

		}

	public:
		error_code& operator=(errc ec)
		{
			this->assign(static_cast<int>(ec), get_error_category());

			return *this;
		}

		const char* category_name()
		{
			return this->category().name();
		}

	public:
		error_code(const error_code&) = default;
		error_code& operator=(const error_code&) = default;

		error_code(error_code&&) = default;
		error_code& operator=(error_code&&) = default;
	};
} // namespace aquarius