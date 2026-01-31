#pragma once
#include <aquarius/tbl/engine/field.hpp>
#include <chrono>
#include <format>


using namespace std::chrono_literals;

namespace aquarius
{
	namespace tbl
	{
		template <auto Tag, std::size_t I, typename... Args>
		class basic_datetime : public fields<Args...>
		{
			static_assert(I < 7, "datetime second precision must be less than 7");

		public:
			basic_datetime() = default;

			basic_datetime(time_t v)
				: value_(std::chrono::system_clock::from_time_t(v))
			{}

			template <typename TimePoint>
			basic_datetime(const TimePoint& v)
				: value_(v)
			{}

		public:
			time_t to_time_t() const
			{
				return std::chrono::system_clock::to_time_t(value_);
			}

			std::string to_time() const
			{
				std::chrono::hh_mm_ss hms(value_ - std::chrono::floor<std::chrono::days>(value_));

				auto time_str = std::format("'{:%T}'", hms);

				auto pos = time_str.find(".");
				if (pos != std::string::npos)
				{
					time_str = time_str.substr(0, pos + I + 1);
				}

				return time_str;
			}

			std::string to_date() const
			{
				std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(value_));

				return std::format("'{:%F}'", ymd);
			}

			std::string to_datetime() const
			{
				return std::format("'{} {}'", to_date(), to_time());
			}

		protected:
			std::chrono::system_clock::time_point value_;
		};

		template <std::size_t I, typename... Args>
		class _time : public basic_datetime<0, I, Args...>
		{
			using base_type = basic_datetime<0, I, Args...>;

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
				return this->to_time();
			}

		public:
			constexpr static auto get_type_name()
			{
				constexpr static auto type_name = " time("sv;
				constexpr static auto end = ")"sv;

				return concat_v<type_name, int_to_string<I>(), end>;
			}
		};

		template <std::size_t I, typename... Args>
		std::ostream& operator<<(std::ostream& os, const _time<I, Args...>& v)
		{
			os << static_cast<std::string>(v);

			return os;
		}

		template <std::size_t I, typename... Args>
		std::istream& operator>>(std::istream& is, _time<I, Args...>& v)
		{
			time_t t{};
			is >> t;

			v = _time<I, Args...>(t);

			return is;
		}

		template <typename... Args>
		class timestamp : public basic_datetime<0, 0, Args...>
		{
			using base_type = basic_datetime<0, 0, Args...>;

		public:
			timestamp() = default;

			timestamp(time_t v)
				: base_type(v)
			{}

			timestamp(const std::chrono::system_clock::time_point& v)
				: base_type(v)
			{}

			operator std::string() const
			{
				return this->to_time_t();
			}

		public:
			constexpr static auto get_type_name()
			{
				return " timestamp"sv;
			}
		};

		template <typename... Args>
		std::ostream& operator<<(std::ostream& os, const timestamp<Args...>& v)
		{
			os << static_cast<std::string>(v);

			return os;
		}

		template <typename... Args>
		std::istream& operator>>(std::istream& is, timestamp<Args...>& v)
		{
			time_t t{};
			is >> t;

			v = timestamp<Args...>(t);

			return is;
		}

		template <typename... Args>
		class date : public basic_datetime<1, 0, Args...>
		{
			using base_type = basic_datetime<1, 0, Args...>;

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

			void set_value(std::stringstream& ss)
			{
				std::string value{};
				ss >> value;

				*this = date<Args...>(value);
			}

		public:
			constexpr static auto get_type_name()
			{
				return " date"sv;
			}
		};

		template <typename... Args>
		std::ostream& operator<<(std::ostream& os, const date<Args...>& v)
		{
			os << static_cast<std::string>(v);

			return os;
		}

		template <typename... Args>
		std::istream& operator>>(std::istream& is, date<Args...>& v)
		{
			is << static_cast<std::string>(v);

			std::string value{};
			is >> value;

			tm t{};
			

			v = date<Args...>();

			return is;
		}

		template <std::size_t I, typename... Args>
		class datetime : public basic_datetime<0, I, Args...>
		{
			using base_type = basic_datetime<0, I, Args...>;

		public:
			datetime() = default;

			datetime(time_t v)
				: base_type(v)
			{}

			datetime(const std::chrono::system_clock::time_point& v)
				: base_type(v)
			{}

			operator std::string() const
			{
				return this->to_datetime();
			}

			void set_value(std::stringstream& ss)
			{
				tm t{};
				ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");

				*this = datetime<I, Args...>(std::chrono::system_clock::from_time_t(std::mktime(&t)));
			}

		public:
			constexpr static auto get_type_name()
			{
				constexpr static auto type_name = " datetime("sv;
				constexpr static auto end = ")"sv;

				constexpr static auto number_str = int_to_string<I>();

				return concat_v<type_name, number_str, end>;
			}
		};

		template <std::size_t I, typename... Args>
		std::ostream& operator<<(std::ostream& os, const datetime<I, Args...>& v)
		{
			os << static_cast<std::string>(v);

			return os;
		}
	} // namespace tbl
} // namespace aquarius