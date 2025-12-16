#pragma once
#include <chrono>
#include <format>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace sql
	{
		namespace detail
		{
			struct stamp
			{
				constexpr static auto name = "timestamp";

				auto operator()(time_t t)
				{
					auto pt = std::chrono::system_clock::from_time_t(t);

					auto tm = std::chrono::current_zone()->to_local(pt);

					return std::format("{:%F %T %Z}", tm);
				}
			};

			struct year
			{
				constexpr static auto name = "year";

				int operator()(time_t t)
				{
					auto pt = std::chrono::system_clock::from_time_t(t);

					std::chrono::year_month_day ymd = std::chrono::floor<std::chrono::days>(pt);

					return ymd.year() > 2155y ? 2155 : static_cast<int>(ymd.year());
				}
			};

			struct date
			{
				constexpr static auto name = "date";

				auto operator()(time_t t)
				{
					auto pt = std::chrono::system_clock::from_time_t(t);

					std::chrono::year_month_day ymd = std::chrono::floor<std::chrono::days>(pt);

					return std::format("{:%F}", ymd);
				}
			};

			template <typename T>
			struct time
			{
				constexpr static auto name = "time";

				auto operator()(time_t t)
				{
					auto pt = std::chrono::system_clock::from_time_t(t);

					auto hms = std::chrono::floor<std::chrono::seconds>(pt);

					return std::format("{:%T}", hms);
				}
			};

			template <>
			struct time<int>
			{
				constexpr static auto name = "time";

				auto operator()(int range, time_t t)
				{
					auto pt = std::chrono::system_clock::from_time_t(t);

					auto hms = std::chrono::floor<std::chrono::seconds>(pt);

					return std::format("{} {:%T}", range, hms);
				}
			};

			struct datetime_impl
			{
				constexpr static auto name = "datetime";

				auto operator()(time_t t)
				{
					auto pt = std::chrono::system_clock::from_time_t(t);
					auto hms = std::chrono::floor<std::chrono::seconds>(pt);
					return std::format("{:%F}", hms);
				}
			};
		} // namespace detail

		template <typename T>
		class basic_datetime
		{
		public:
			constexpr static auto name = T::name;

		public:
			basic_datetime() = default;

			basic_datetime(time_t stamp)
				: stamp_(stamp)
			{}

			auto operator()() const
			{
				return std::declval<T>()(stamp_);
			}

		private:
			time_t stamp_;
		};
	} // namespace sql
} // namespace aquarius