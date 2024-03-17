#pragma once
#include <aquarius/system/error_code/error_category.hpp>
#include <system_error>

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

		error_code(int value)
			: value_(value)
			, cat_(&system::system_category())
		{

		}

		error_code(system_errc value)
			: value_(static_cast<int>(value))
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

		error_code& operator=(system_errc value) noexcept
		{
			value_ = static_cast<int>(value);

			return *this;
		}

		bool operator==(const system_errc& errc)
		{
			return value_ == static_cast<int>(errc);
		}

		std::string category() const
		{
			return cat_->name();
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