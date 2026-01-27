#pragma once
#include <aquarius/tbl/engine/field.hpp>
#include <limits>
#include <string_view>

namespace aquarius
{
	namespace tbl
	{
		template <typename T, typename... Args>
		requires std::is_floating_point_v<T>
		class basic_float : public field<Args...>
		{
		public:
			basic_float() = default;

			basic_float(T v)
				: value_(v)
			{}

			basic_float(const basic_float& v)
				: value_(v.value_)
			{}

			operator T() const
			{
				return value_;
			}

		protected:
			T value_;
		};

		template <typename... Args>
		class _float : public basic_float<float, Args...>
		{
		public:
			constexpr static auto get_type_name()
			{
				return " float";
			}

		public:
			_float() = default;

			_float(float v)
				: basic_float<float, Args...>(v)
			{}

		public:
			void set_value(std::stringstream& ss)
			{
				ss >> this->value_;
			}
		};

		template <typename... Args>
		class _double : public basic_float<double, Args...>
		{
		public:
			constexpr static auto get_type_name()
			{
				return " double";
			}

		public:
			_double() = default;

			_double(double v)
				: basic_float<double, Args...>(v)
			{}

		public:
			void set_value(std::stringstream& ss)
			{
				ss >> this->value_;
			}
		};
	} // namespace tbl
} // namespace aquarius