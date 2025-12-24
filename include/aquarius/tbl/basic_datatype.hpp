#pragma once
#include <array>
#include <vector>
#include <aquarius/tbl/engine/varchar.hpp>

namespace aquarius
{
	namespace tbl
	{
		template <typename T, template <typename> typename Impl, typename Tag = T>
		class basic_datatype
		{
		public:
			constexpr static auto name = Impl<Tag>::name;

			constexpr static auto max_length = Impl<Tag>::max_length;

		public:
			basic_datatype()
				: value_()
			{}

			template <typename U>
			requires(std::convertible_to<U, T>)
			basic_datatype(const U& value)
				: value_(static_cast<T>(value))
			{}

			basic_datatype& operator=(const basic_datatype& i)
			{
				if (this != std::addressof(i))
				{
					value_ = i.value_;
				}

				return *this;
			}

			basic_datatype& operator=(basic_datatype&& i) noexcept
			{
				if (this != std::addressof(i))
				{
					if constexpr (std::is_trivial_v<T>)
					{
						value_ = std::exchange(i.value_, T{});
					}
					else
					{
						value_ = std::move(i.value_);
					}
				}

				return *this;
			}

			template <typename U>
			requires(std::convertible_to<U, T>)
			basic_datatype& operator=(const U& value)
			{
				value_ = static_cast<T>(value);

				return *this;
			}

			template <typename U>
			requires(std::convertible_to<U, T>)
			basic_datatype& operator=(U&& value) noexcept
			{
				value_ = std::move(value);

				return *this;
			}

			operator T() const
			{
				return static_cast<T>(value_);
			}

		protected:
			T value_;
		};

		template <auto N>
		class basic_char : public basic_datatype<std::array<char, N>, _char>
		{
			using base_type = basic_datatype<std::array<char, N>, _char>;

		public:
			basic_char()
				: basic_datatype<std::array<char, N>, _char>()
			{}

			basic_char(const char* value)
			{
				auto size = std::strlen(value);

				size = std::min<std::size_t>(size, N - 1);

				std::copy(value, value + size, this->value_.begin());
			}
		};

		template <typename Tag, template <typename> typename Impl, typename T = std::vector<char>>
		class basic_text : public basic_datatype<T, Impl, Tag>
		{
		public:
			basic_text(const char* value)
			{
				auto size = std::strlen(value);

				std::copy(value, value + size, this->value_.begin());
			}
		};
	} // namespace tbl
} // namespace aquarius