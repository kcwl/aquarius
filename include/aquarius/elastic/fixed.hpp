#pragma once
#include <array>
#include <optional>
#include <vector>

namespace aquarius
{
	template <typename _Ty>
	class fixed
	{
	public:
		fixed() = default;

		fixed(_Ty value)
			: value_(value)
		{

		}

	public:
		using value_type = _Ty;

		template <typename _Integer>
		fixed& operator=(const _Integer& other)
		{
			value_ = other;

			return *this;
		}

		template <typename _U>
		bool operator==(const fixed<_U>& other) const
		{
			return value_ == other.value_;
		}

		value_type& operator*()
		{
			return value_;
		}

	private:
		value_type value_;
	};
} // namespace elastic