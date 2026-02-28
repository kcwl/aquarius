#pragma once
#include <chrono>
#include <string>
#include <sstream>

namespace aquarius
{
	namespace tbl
	{
		namespace detail
		{
			template <typename T>
			class fields
			{
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

			protected:
				void make_condition()
				{
					flag_ |= 1;
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

			protected:
				T value_;

			private:
				int32_t flag_;
			};

			template <auto Tag, std::size_t I>
			class basic_datetime : public fields<std::chrono::system_clock::time_point>
			{
				static_assert(I < 7, "datetime second precision must be less than 7");

				using base_type = fields<std::chrono::system_clock::time_point>;

			public:
				basic_datetime() = default;

				basic_datetime(time_t v)
					: basic_datetime(std::chrono::system_clock::from_time_t(v))
				{}

				basic_datetime(std::chrono::system_clock::time_point tp)
					: base_type(tp)
				{}

			public:
				void set_condition(time_t v)
				{
					this->make_condition();

					this->value_ = std::chrono::system_clock::from_time_t(v);
				}

				void set_value(time_t v)
				{
					this->make_value();

					this->value_ = std::chrono::system_clock::from_time_t(v);
				}

				virtual void serialize(std::stringstream& ss)
				{
					tm t{};
					ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");

					this->value_ = std::chrono::system_clock::from_time_t(std::mktime(&t));
				}
			};
		} // namespace detail

		class integer : public detail::fields<uint64_t>
		{
		public:
			integer() = default;

			template<typename T>
			requires(std::is_integral_v<T>)
			integer(const T& v)
				: fields(v)
			{

			}

			operator uint64_t() const
			{
				return this->value_;
			}

			virtual void serialize(std::stringstream& ss)
			{
				ss >> value_;
			}
		};

		class string : public detail::fields<std::string>
		{
			using base_type = detail::fields<std::string>;

		public:
			string() = default;

			string(const std::string& v)
				: base_type(v)
			{}

			string(const char* value)
				: base_type(std::string(value))
			{}

		public:
			operator std::string() const
			{
				return "'" + this->value_ + "'";
			}

			virtual void serialize(std::stringstream& ss)
			{
				ss >> value_;
			}

		public:
			void set_condition(const char* v)
			{
				return set_condition(std::string(v));
			}

			void set_value(const char* v)
			{
				return set_condition(std::string(v));
			}

			void set_condition(const std::string& v)
			{
				this->make_value();

				this->value_ = v;
			}

			void set_value(const std::string& v)
			{
				this->make_value();

				this->value_ = v;
			}
		};

		inline std::ostream& operator<<(std::ostream& os, const string& v)
		{
			return os << static_cast<std::string>(v);
		}

		class _float : public detail::fields<float>
		{
		public:
			_float() = default;

			operator float() const
			{
				return this->value_;
			}

		public:
			virtual void serialize(std::stringstream& ss)
			{
				ss >> value_;
			}
		};

		class _double : public detail::fields<double>
		{
		public:
			_double() = default;

		public:
			operator double() const
			{
				return this->value_;
			}

		public:
			virtual void serialize(std::stringstream& ss)
			{
				ss >> value_;
			}
		};

		template <std::size_t I>
		class _time : public detail::basic_datetime<0, I>
		{
			using base_type = detail::basic_datetime<0, I>;

		public:
			_time() = default;

			_time(time_t v)
				: base_type(v)
			{}

			_time(const std::chrono::system_clock::time_point& v)
				: base_type(v)
			{}

			operator std::string() const
			{
				return to_time();
			}

		private:
			std::string to_time() const
			{
				std::chrono::hh_mm_ss hms(this->value_ - std::chrono::floor<std::chrono::days>(this->value_));

				auto time_str = std::format("'{:%T}'", hms);

				auto pos = time_str.find(".");
				if (pos != std::string::npos)
				{
					time_str = time_str.substr(0, pos + I + 1);
				}

				return time_str;
			}
		};

		template <std::size_t I>
		std::ostream& operator<<(std::ostream& os, const _time<I>& v)
		{
			os << static_cast<std::string>(v);

			return os;
		}

		class timestamp : public detail::basic_datetime<1, 0>
		{
			using base_type = detail::basic_datetime<1, 0>;

		public:
			timestamp() = default;

			timestamp(time_t v)
				: base_type(v)
			{}

			operator time_t() const
			{
				return to_time_t();
			}

		private:
			time_t to_time_t() const
			{
				return std::chrono::system_clock::to_time_t(this->value_);
			}
		};

		inline std::ostream& operator<<(std::ostream& os, const timestamp& v)
		{
			os << static_cast<time_t>(v);

			return os;
		}

		class date : public detail::basic_datetime<2, 0>
		{
			using base_type = basic_datetime<2, 0>;

		public:
			date() = default;

			date(time_t v)
				: base_type(v)
			{}

			date(const std::string& v)
				: base_type()
			{
				std::stringstream ss(v);

				tm t{};
				ss >> std::get_time(&t, "%Y-%m-%d");

				this->value_ = std::chrono::system_clock::from_time_t(std::mktime(&t));
			}

			date(const std::chrono::system_clock::time_point& v)
				: base_type(v)
			{}

			operator std::string() const
			{
				return this->to_date();
			}

		private:
			std::string to_date() const
			{
				std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(this->value_));

				return std::format("'{:%F}'", ymd);
			}
		};

		inline std::ostream& operator<<(std::ostream& os, const date& v)
		{
			os << static_cast<std::string>(v);

			return os;
		}

		class datetime : public detail::basic_datetime<3, 0>
		{
			using base_type = detail::basic_datetime<3, 0>;

		public:
			datetime() = default;

			datetime(time_t v)
				: base_type(v)
			{}

			operator std::string() const
			{
				return this->to_datetime();
			}

			datetime(const std::string& v)
			{
				std::stringstream ss;

				ss << v;

				base_type::serialize(ss);
			}

		private:
			std::string to_datetime() const
			{
				std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(value_));

				std::chrono::hh_mm_ss hms(value_ - std::chrono::floor<std::chrono::days>(value_));

				return std::format("'{%F} {%T}'", ymd, hms);
			}
		};

		inline std::ostream& operator<<(std::ostream& os, const datetime& v)
		{
			os << static_cast<std::string>(v);

			return os;
		}

		//class binary : public detail::fields<std::vector<uint8_t>>
		//{
		//public:
		//	binary() = default;

		//	template <typename T>
		//	requires std::convertible_to<T, uint8_t>
		//	binary(const std::vector<uint8_t>& v)
		//		: binary(v)
		//	{}

		//	template <typename T>
		//	requires std::convertible_to<T, uint8_t>
		//	binary(std::initializer_list<T> v)
		//	{
		//		std::copy(v.begin(), v.end(), std::back_inserter(this->value_));
		//	}

		//	std::ostream& operator<<(std::ostream& os) const
		//	{
		//		for (auto& s : value_)
		//		{
		//			os << (int)s;
		//		}

		//		return os;
		//	}

		//	virtual void serialize(std::stringstream& ss) override
		//	{
		//		return;
		//	}

		//private:
		//	std::vector<uint8_t> value_;
		//};

		//std::ostream& operator<<(std::ostream& os, const binary& v)
		//{
		//	v << os;

		//	return os;
		//}
	} // namespace mysql
} // namespace aquarius