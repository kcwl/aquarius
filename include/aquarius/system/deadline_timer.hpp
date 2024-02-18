#pragma once
#include <aquarius/system/asio.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename _Time>
		struct time_traits;

		template <>
		struct time_traits<std::chrono::system_clock::time_point>
		{
			using time_type = std::chrono::system_clock::time_point;

			using duration_type = std::chrono::system_clock::duration;

			static time_type now()
			{
				return std::chrono::system_clock::now();
			}

			static time_type add(const time_type& t, const duration_type& d)
			{
				return t + d;
			}

			static duration_type subtract(const time_type& t1, const time_type& t2)
			{
				return t1 - t2;
			}

			static bool less_than(const time_type& t1, const time_type& t2)
			{
				return t1 < t2;
			}

			static boost::posix_time::time_duration to_posix_duration(const duration_type& d)
			{
				auto hour = std::chrono::duration_cast<std::chrono::hours>(d);

				auto min = std::chrono::duration_cast<std::chrono::minutes>(d - hour);

				auto sec = std::chrono::duration_cast<std::chrono::seconds>(d - hour - min);

				return boost::posix_time::time_duration(hour.count(), min.count(), min.count());
			}
		};

		using deadline_timer = asio::basic_deadline_timer<std::chrono::system_clock::time_point,
														  time_traits<std::chrono::system_clock::time_point>>;

		using steady_timer = asio::steady_timer;
	} // namespace detail
} // namespace aquarius