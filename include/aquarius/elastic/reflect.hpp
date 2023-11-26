#pragma once
#include "type_traits.hpp"

#include <string_view>

using namespace std::string_view_literals;

namespace
{
	template <typename _Ty, std::size_t N>
	constexpr std::size_t find_n_of()
	{
		std::size_t pos = 0;
		constexpr auto member = _Ty::member_str();

		int i = N + 1;
		for (; i > 0; i--)
		{
			pos = member.find_first_of(";", pos + 1);
		}

		return pos;
	}

	template <typename _Ty, std::size_t _Index>
	constexpr auto get()
	{
		constexpr auto member_info = _Ty::member_str();

		constexpr std::size_t pos = member_info.find_first_of(";", find_n_of<_Ty, _Index>());

		constexpr std::string_view s =
			member_info.substr(find_n_of<_Ty, _Index - 1>() + 1, pos - find_n_of<_Ty, _Index - 1>() - 1);

		constexpr std::string_view result = s.substr(s.rfind(" ") + 1);

		return result;
	}

	template <typename _Ty, std::size_t... I>
	constexpr auto split_impl(std::index_sequence<I...>)
	{
		return std::array{ get<_Ty, I>()... };
	}

	template <typename _Ty, std::size_t N, typename Indices = std::make_index_sequence<N>>
	constexpr auto split()
	{
		return split_impl<_Ty>(Indices{});
	}
}

namespace aquarius
{
	template <std::size_t N>
	using size_t_ = std::integral_constant<std::size_t, N>;

	namespace detail
	{
		using one_range = std::true_type;
		using multi_range = std::false_type;

		template <std::size_t Begin, std::size_t End>
		using is_one_element_range = std::bool_constant<Begin == End>;

		template <typename _Ty, std::size_t Start, std::size_t End>
		constexpr std::size_t detect_fields_greey(one_range)
		{
			static_assert(Start == End, "It is not a correct way with binary search!");

			if constexpr (aggregate_inialize_t<_Ty, Start>)
			{
				return Start;
			}
			else
			{
				return 0;
			}
		}

		template <typename _Ty, std::size_t Begin, std::size_t End>
		constexpr std::size_t detect_fields_greey(multi_range);

		template <typename _Ty, std::size_t Begin, std::size_t End>
		requires(aggregate_inialize_t<_Ty, End>)
		constexpr auto detect_fields_greey(multi_range)
		{
			constexpr std::size_t next = End + (End - Begin + 1) / 2;

			return detail::template detect_fields_greey<_Ty, End, next>(is_one_element_range<End, next>{});
		}

		template <typename _Ty, std::size_t Begin, std::size_t End>
		constexpr std::size_t detect_fields_greey(multi_range)
		{
			constexpr std::size_t middle = Begin + (End - Begin) / 2;

			return detail::template detect_fields_greey<_Ty, Begin, middle>(is_one_element_range<Begin, middle>{});
		}

		template <typename _Ty, std::size_t N>
		constexpr std::size_t detect_fields_count_dispatch(size_t_<N>)
		{
			return detail::template detect_fields_greey<_Ty, 0, N>(multi_range{});
		}

		template <tuple_t _Ty>
		constexpr std::size_t fields_count()
		{
			using type = std::remove_cv_t<_Ty>;

			return std::tuple_size_v<type>;
		}

		template <typename _Ty>
		constexpr std::size_t fields_count()
		{
			using type = std::remove_cv_t<_Ty>;

			constexpr std::size_t max_fields_count = sizeof(type) * CHAR_BIT;

			constexpr std::size_t result =
				detail::template detect_fields_count_dispatch<type>(size_t_<max_fields_count>{});

			return result;
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&&, size_t_<0>) noexcept
		{
			return std::forward_as_tuple();
		}

		template <class_t _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<1>) noexcept
		{
			auto&& [a] = val;
			return std::forward_as_tuple(a);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<1>) noexcept
		{
			auto&& a = val;
			return std::forward_as_tuple(a);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<2>) noexcept
		{
			auto&& [a, b] = val;
			return std::forward_as_tuple(a, b);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<3>) noexcept
		{
			auto&& [a, b, c] = val;
			return std::forward_as_tuple(a, b, c);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<4>) noexcept
		{
			auto&& [a, b, c, d] = std::forward<_Ty>(val);
			return std::forward_as_tuple(a, b, c, d);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<5>) noexcept
		{
			auto&& [a, b, c, d, e] = val;
			return std::forward_as_tuple(a, b, c, d, e);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<6>) noexcept
		{
			auto&& [a, b, c, d, e, f] = val;
			return std::forward_as_tuple(a, b, c, d, e, f);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<7>) noexcept
		{
			auto&& [a, b, c, d, e, f, g] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<8>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<9>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, i] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, i);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<10>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, i, j] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, i, j);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<11>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, i, j, k] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, i, j, k);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<12>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<13>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<14>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<15>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<16>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<17>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<18>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<19>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<20>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<21>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<22>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<23>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<24>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<25>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<26>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<27>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B,
										 C);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<28>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B,
										 C, D);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<29>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B,
										 C, D, E);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<30>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B,
										 C, D, E, F);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<31>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G] = val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B,
										 C, D, E, F, G);
		}

		template <typename _Ty>
		constexpr auto make_tuple(_Ty&& val, size_t_<32>) noexcept
		{
			auto&& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H] =
				val;
			return std::forward_as_tuple(a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B,
										 C, D, E, F, G, H);
		}
	} // namespace detail

	template <typename _Ty>
	using tuple_size = size_t_<detail::fields_count<_Ty>()>;

	template <typename _Ty>
	constexpr std::size_t tuple_size_v = tuple_size<std::remove_cvref_t<_Ty>>::value;

	template <typename _Ty>
	constexpr auto name() -> std::string_view
	{
		using namespace std::string_view_literals;

#ifndef __linux
		constexpr std::string_view name = __FUNCSIG__ ""sv;

		constexpr auto left_bracket = name.find_last_of("<");

		constexpr auto end_bracket = name.find_last_of(">");

		constexpr auto temp_name = name.substr(left_bracket + 1, end_bracket - left_bracket - 1);

		constexpr auto start = name.find_last_of(":");

		if constexpr (start == std::string_view::npos)
		{
			return temp_name;
		}
		else
		{
			return name.substr(start + 1, end_bracket - start - 1);
		}
#else
		constexpr std::string_view name = __PRETTY_FUNCTION__;

		constexpr auto left_bracket = name.find_last_of("[");
		constexpr auto right_bracket = name.find_last_of("]");
		constexpr auto name_in_bracket = name.substr(left_bracket + 1, right_bracket - left_bracket - 1);

		constexpr auto left_equ = name_in_bracket.find_first_of("=");
		constexpr auto right_f = name_in_bracket.find_first_of(";");

		constexpr auto first_name = name_in_bracket.substr(left_equ + 2, right_f - left_equ - 2);

		constexpr auto sp = first_name.find_last_of(":");

		if constexpr (sp == std::string_view::npos)
		{
			return first_name;
		}
		else
		{
			return first_name.substr(sp + 1);
		}
#endif
	}

	template <std::size_t N, typename _Ty>
	constexpr auto&& get(_Ty&& val)
	{
		return std::get<N>(detail::template make_tuple(val, size_t_<aquarius::tuple_size_v<_Ty>>{}));
	}

	template <std::size_t I, typename _Tuple>
	struct tuple_element
	{
		using type = decltype(aquarius::get<I>(std::declval<_Tuple>()));
	};

	template <std::size_t I, typename _Tuple>
	using tuple_element_t = typename tuple_element<I, _Tuple>::type;

	template<std::size_t I, typename _Tuple>
	constexpr auto tuple_element_name()
	{
		return std::get<I>(decltype(_Tuple::make_reflect_member())::apply_member());
	}

} // namespace elastic

#define MAKE_REFLECT(...)	\
	template<typename _Ty>\
	static auto make_reflect_member()\
	{\
		struct reflect_member\
		{\
			inline constexpr static auto member_str()\
			{\
				return std::string_view(#__VA_ARGS__,sizeof(#__VA_ARGS__) - 1);\
			}\
			inline constexpr decltype(auto) static apply_member()\
			{\
				return split<reflect_member, aquarius::tuple_size_v<_Ty>>();\
			}\
		};\
		return reflect_member{};\
	}

#define MACRO(...) __VA_ARGS__

#define REFLECT_DEFINE(...) MACRO(__VA_ARGS__) MAKE_REFLECT(__VA_ARGS__)