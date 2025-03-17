#pragma once
#include <aquarius/core/concepts.hpp>
#include <aquarius/core/power.hpp>
#include <aquarius/serialize/access.hpp>
#include <boost/pfr.hpp>
#include <vector>

namespace aquarius
{
	namespace binary
	{
		template <typename _Archive, typename _Ty>
		bool serialize(_Archive& ar, _Ty&& t);

		template <typename _Archive, typename _Ty>
		bool deserialize(_Archive& ar, _Ty& t);
	} // namespace binary

	namespace detail
	{
		template <integer_t _Ty>
		constexpr uint8_t get_symbol(const _Ty t)
		{
			constexpr auto bit = sizeof(_Ty) * 8 - 1;

			constexpr auto pow = power<2, bit>::value;

			return static_cast<uint8_t>((t & pow) >> bit);
		}

		template <integer_t _Ty>
		constexpr auto get_data(const _Ty t)
		{
			constexpr auto pow = power<2, sizeof(_Ty) * 8 - 1>::value - 1;

			return static_cast<_Ty>(t & pow);
		}

		inline uint8_t filter_symbol(const uint8_t value)
		{
			constexpr auto bit = sizeof(uint8_t) * 8 - 1;

			constexpr auto pow = power<2, bit>::value;

			return static_cast<uint8_t>((value & pow) >> bit);
		}

		inline uint8_t filter_length(const uint8_t value)
		{
			constexpr auto pow = power<2, 0>::value + power<2, 1>::value + power<2, 2>::value + power<2, 3>::value;

			return value & pow;
		}

		template <typename _Ty, typename _Archive>
		inline bool serialize(_Archive& ar, _Ty&& value)
		{
			using type = std::remove_cvref_t<_Ty>;

			using value_type = typename _Archive::value_type;

			if constexpr (integer_t<type>)
			{
				uint8_t symbol = 0;

				type result = value;

				type temp = value;

				if constexpr (!std::is_unsigned_v<_Ty>)
				{
					symbol = get_symbol(value);

					if (symbol & 1)
					{
						result = ~result + 1;

						temp = result;
					}
				}

				std::size_t bytes{};

				while (temp > 0)
				{
					temp >>= 8;

					bytes++;
				}

				symbol = symbol << 7 | static_cast<uint8_t>(bytes);

				ar.save(&symbol, 1);

				ar.save((value_type*)&result, bytes);
			}
			else if constexpr (boolean_t<type>)
			{
				char result = static_cast<char>(std::forward<_Ty>(value));

				ar.save((value_type*)&result, 1);
			}
			else if constexpr (enum_t<type>)
			{
				uint64_t result = static_cast<uint64_t>(std::forward<_Ty>(value));

				serialize(ar, result);
			}
			else if constexpr (float_point_t<type>)
			{
				constexpr auto size = sizeof(_Ty);

				using value_type = typename _Archive::value_type;

				ar.save((value_type*)&value, size);
			}
			else if constexpr (string_t<type>)
			{
				using value_type = typename _Archive::value_type;

				auto bytes = value.size();

				serialize(ar, bytes);

				ar.save((value_type*)value.data(), bytes);
			}
			else if constexpr (sequence_t<type>)
			{
				const auto bytes = value.size();

				serialize(ar, bytes);

				for (auto& mem : value)
				{
					binary::serialize(ar, mem);
				}
			}
			else if constexpr (struct_t<_Ty>)
			{
				constexpr auto struct_count = boost::pfr::tuple_size_v<_Ty>;

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ return (serialze(ar, boost::pfr::get<I>(value)), ...); };

				std::apply(f, std::make_index_sequence<struct_count>{});
			}

			return true;
		}

		template <typename _Ty, typename _Archive>
		inline bool deserialize(_Archive& ar, _Ty& t)
		{
			using value_type = typename _Archive::value_type;

			if constexpr (integer_t<_Ty>)
			{
				value_type c{};
				if (!ar.load((value_type*)&c, 1))
					return false;

				uint8_t symbol = filter_symbol(c);

				auto length = filter_length(c);

				if (!ar.load((value_type*)&t, length))
					return false;

				symbol == 0 ? t : t = ~t + 1;
			}
			else if constexpr (boolean_t<_Ty>)
			{
				char temp{};

				if (!ar.load((value_type*)&temp, 1))
					return false;

				t = static_cast<bool>(temp);
			}
			else if constexpr (enum_t<_Ty>)
			{
				uint64_t temp{};

				if (!deserialize(ar, temp))
					return false;

				t = static_cast<_Ty>(temp);
			}
			else if constexpr (float_point_t<_Ty>)
			{
				constexpr auto size = sizeof(_Ty);

				if (!ar.load((value_type*)&t, size))
					return false;
			}
			else if constexpr (string_t<_Ty>)
			{
				std::size_t bytes{};

				if (!deserialize(ar, bytes))
					return false;

				t.resize(bytes);

				if (!ar.load((value_type*)t.data(), bytes))
					return false;
			}
			else if constexpr (sequence_t<_Ty>)
			{
				int bytes{};

				if (!deserialize(ar, bytes))
					return false;

				while (bytes--)
				{
					t.push_back({});

					auto& back = t.back();

					if (!binary::deserialize(ar, back))
						return false;
				}
			}
			else if constexpr (struct_t<_Ty>)
			{
				constexpr auto struct_count = boost::pfr::tuple_size_v<_Ty>;

				auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ return (deserialze(ar, boost::pfr::get<I>(t)), ...); };

				std::apply(f, std::make_index_sequence<struct_count>{});
			}

			return true;
		}
	} // namespace detail

	namespace binary
	{
		template <typename _Archive, typename _Ty>
		inline bool deserialize(_Archive& ar, _Ty& t)
		{
			if constexpr (non_inherit_t<_Ty>)
			{
				return detail::template deserialize<_Ty>(ar, t);
			}
			else
			{
				access::template serialize(ar, t);

				return true;
			}
		}

		template <typename _Archive, typename _Ty>
		inline bool serialize(_Archive& ar, _Ty&& t)
		{
			if constexpr (non_inherit_t<_Ty>)
			{
				return detail::template serialize(ar, std::forward<_Ty>(t));
			}
			else
			{
				access::template serialize(ar, std::forward<_Ty>(t));

				return true;
			}
		}
	} // namespace binary
} // namespace aquarius