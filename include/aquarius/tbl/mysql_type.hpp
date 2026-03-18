#pragma once
#include <chrono>
#include <sstream>
#include <string>
#include <ostream>

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		class fields
		{
		public:
			friend std::ostream& operator<<(std::ostream& os, const fields<T>& f)
			{
				os << f.value_;

				return os;
			}

			friend std::istream& operator>>(std::istream& is, fields<T>& f)
			{
				is >> f.value_;

				return is;
			}

		public:
			fields()
				: value_()
				, flag_(0)
			{}

			fields(const T& value)
				: value_(value)
				, flag_(0)
			{}

			fields(const fields& other)
				: value_(other.value_)
				, flag_(other.flag_)
			{}

			fields(fields&& other) noexcept
				: value_(std::exchange(other.value_, {}))
				, flag_(std::exchange(other.flag_, 0))
			{}

			fields& operator=(const fields& other)
			{
				if (this != &other)
				{
					value_ = other.value_;
					flag_ = other.flag_;
				}

				return *this;
			}

			fields& operator=(fields&& other) noexcept
			{
				if (this != &other)
				{
					value_ = std::exchange(other.value_, {});
					flag_ = std::exchange(other.flag_, 0);
				}

				return *this;
			}

			virtual ~fields() = default;

		public:
			bool has_condition() const
			{
				return condition();
			}

			bool has_value() const
			{
				return value_tag();
			}

		public:
			void set_condition(const T& v)
			{
				this->make_condition();

				this->value_ = v;
			}

			void set_value(T&& v)
			{
				this->make_value();

				this->value_ = v;
			}

			void clear()
			{
				clear_condition();

				this->value_ = {};
			}

		protected:
			void make_condition()
			{
				flag_ |= 1;
			}

			void clear_condition()
			{
				flag_ &= 0;
			}

			void make_value()
			{
				flag_ |= 1 << 2;
			}

			bool condition() const
			{
				return (flag_ & 1) != 0;
			}

			bool value_tag() const
			{
				return ((flag_ >> 2) & 0x1) != 0;
			}

		private:
			int32_t flag_;

			T value_;
		};
	} // namespace tbl
} // namespace aquarius