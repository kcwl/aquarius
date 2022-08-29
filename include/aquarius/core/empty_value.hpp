#pragma once
#include <cstddef>
#include "type_traits.hpp"

namespace aquarius
{
	namespace core
	{
		struct empty_init_t {};

		template<typename _Ty, std::size_t N = 0, bool E = use_empty_value_base_v<_Ty>>
		class empty_value
		{
		public:
			empty_value() = default;

			empty_value(empty_init_t)
				: value_()
			{

			}

			template<typename _U, typename... _Args>
			empty_value(empty_init_t, _U&& value, _Args&&... args)
				: value_(std::forward<_U>(value), std::forward<_Args>(args)...)
			{

			}

			_Ty& get() noexcept
			{
				return value_;
			}

			_Ty& get() const noexcept
			{
				return value_;
			}

		private:
			_Ty value_;
		};

		template<typename _Ty, std::size_t N>
		class empty_value<_Ty, N, true> : private _Ty
		{
		public:
			empty_value() = default;

			empty_value(empty_init_t)
				: _Ty()
			{

			}

			template<typename _U, typename... _Args>
			empty_value(empty_init_t, _U&& value, _Args&&... args)
				: _Ty(std::forward<_U>(value), std::forward<_Args>(args)...)
			{

			}

			_Ty& get() noexcept
			{
				return *this;
			}

			const _Ty& get() const noexcept
			{
				return *this;
			}
		};

		inline static constexpr  empty_init_t empty_init = empty_init_t{};
	}
}