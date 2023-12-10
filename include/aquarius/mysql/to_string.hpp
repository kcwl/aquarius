#pragma once
#include <array>
#include <string_view>

namespace aquarius
{
	namespace impl
	{
		template <int A, int B>
		struct get_power
		{
			constexpr static std::size_t value = A * get_power<A, B - 1>::value;
		};

		template <int A>
		struct get_power<A, 0>
		{
			constexpr static std::size_t value = 1;
		};

		template <std::size_t N, std::size_t I>
		constexpr char to_char()
		{
			constexpr auto q = get_power<10, I>::value;
			constexpr auto t = N / q;
			return '0' + t % 10;
		}

		template <std::size_t N>
		struct count
		{
			constexpr static std::size_t value = count<N / 10>::value + 1;
		};

		template <>
		struct count<0>
		{
			constexpr static std::size_t value = 0;
		};

		template <std::size_t N, std::size_t C, std::size_t... I>
		constexpr auto to_array(std::index_sequence<I...>)
		{
			return std::array{ to_char<N, C - I - 1>()... };
		}

		template <std::size_t N>
		constexpr auto to_array()
		{
			return to_array<N, count<N>::value>(std::make_index_sequence<count<N>::value>{});
		}

		template <typename _Ty, _Ty value>
		constexpr auto to_string_view()
		{
			constexpr auto ste_view = std::string_view(value.data(), value.size());

			return ste_view;
		}
	}

	template <std::size_t N>
	struct to_string
	{
	private:
		constexpr static auto arr = impl::to_array<N>();

	public:
		constexpr static auto value = impl::to_string_view<decltype(arr), arr>();
	};
}