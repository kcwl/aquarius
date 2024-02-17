#pragma once
#include <system_error>
#include <aquarius/system/error_category.hpp>

namespace aquarius
{
	class error_code
	{
	public:
		error_code() noexcept
			: value_(0)
			, cat_(&system::system_category())
		{

		}

		error_code(const error_code& ec)
			: value_(ec.value_)
			, cat_(ec.cat_)
		{

		}

	public:
		operator bool() const noexcept
		{
			return value_ != 0;
		}

		error_code& operator=(system::system_errc value) noexcept
		{
			value_ = static_cast<int>(value);

			return *this;
		}

	public:
		int value()
		{
			return value_;
		}

		std::string message()
		{
			return cat_->message(value_);
		}

	private:
		int value_;

		const std::error_category* cat_;
	};
}