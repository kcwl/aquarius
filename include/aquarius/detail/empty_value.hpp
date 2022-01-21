#pragma once
#include <cstddef>
#include "../type_traits.hpp"

namespace aquarius
{
	namespace detail
	{
		struct empty_init_t {};

		template<typename T, std::size_t N = 0, bool E = aquarius::use_empty_value_base_v<T>>
		class empty_value
		{
		public:
			empty_value(empty_init_t)
				: value_()
			{

			}

			template<typename U, typename... Args>
			empty_value(empty_init_t, U&& value, Args&&... args)
				: value_(std::forward<U>(value), std::forward<Args>(args)...)
			{

			}

			T& get() noexcept
			{
				return value_;
			}

			const T& get() noexcept
			{
				return value_;
			}

		private:
			T value_;
		};

		template<typename T, std::size_t N>
		class empty_value<T, N, true> : private T
		{
		public:
			empty_value(empty_init_t)
				: T()
			{

			}

			template<typename U, typename... Args>
			empty_value(empty_init_t, U&& value, Args&&... args)
				: T(std::forward<U>(value), std::forward<Args>(args)...)
			{

			}

			T& get() noexcept
			{
				return *this;
			}

			const T& get() const noexcept
			{
				return *this;
			}
		};

		inline static constexpr  empty_init_t empty_init = empty_init_t{};
	}
}