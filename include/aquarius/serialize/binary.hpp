#pragma once
#include <span>
#include <aquarius/concepts/concepts.hpp>
#include <aquarius/virgo/reflection.hpp>
#include <boost/asio/buffer.hpp>

namespace aquarius
{
	namespace serialize
	{
		struct binary_parse
		{
			template <integer_t T, typename V>
			void to_datas(const T& value, detail::flex_buffer<V>& buff)
			{
				auto temp = value;

				while (temp >= 0x80)
				{
					buff.put(static_cast<char>(temp | 0x80));

					temp >>= 7;
				}
				buff.put(static_cast<char>(temp));
			}

			template <zig_zag T, typename V>
			void to_datas(const T& value, detail::flex_buffer<V>& buff)
			{
				auto temp = (value << 1) ^ (value >> (sizeof(T) * 8 - 1));

				return to_datas<uint64_t>(temp, buff);
			}

			template <pod_t T, typename V>
			void to_datas(T value, detail::flex_buffer<V>& buff)
			{
				constexpr auto size = sizeof(T);

				if (size > buff.remain())
					return;

				std::copy((char*)&value, (char*)&value + size, buff.wdata());

				buff.commit(size);
			}

			template <repeated_t T, typename V>
			void to_datas(const T& value, detail::flex_buffer<V>& buff)
			{
				to_datas(value.size(), buff);

				for (auto& c : value)
				{
					to_datas(c, buff);
				}
			}

			template <string_t T, typename V>
			void to_datas(const T& value, detail::flex_buffer<V>& buff)
			{
				to_datas(value.size(), buff);

				if (value.size() > buff.remain())
					return;

				std::copy(std::begin(value), std::end(value), buff.wdata());

				buff.commit(value.size());
			}

			template <reflectable T, typename V>
			void to_datas(const T& value, detail::flex_buffer<V>& buff)
			{
				auto to_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ (to_datas(virgo::reflect::get<I, T>(value), buff), ...); };

				to_binary_impl(std::make_index_sequence<virgo::reflect::tuple_size_v<T>>{});
			}

			template <integer_t T, typename V>
			auto from_datas(detail::flex_buffer<V>& buff) -> T
			{
				T value = static_cast<uint8_t>(buff.get());
				if (value >= 0x80)
				{
					value -= 0x80;

					int8_t temp_bit = 7;

					while (buff.peek() < 0x7f)
					{
						auto s = buff.get();

						if ((s & 0x80) != 0)
						{
							value += static_cast<T>(s & 0x7f) << temp_bit;

							temp_bit += 7;
						}
						else
						{
							value += (static_cast<T>(s) << temp_bit);
							break;
						}
					}
				}

				return value;
			}

			template <zig_zag T, typename V>
			auto from_datas(detail::flex_buffer<V>& buff) -> T
			{
				auto temp = static_cast<T>(from_datas<uint64_t>(buff));

				return static_cast<T>(static_cast<int64_t>((temp >> 1) ^ (-(temp & 0x1))));
			}

			template <pod_t T, typename V>
			auto from_datas(detail::flex_buffer<V>& buff) -> T
			{
				constexpr auto size = sizeof(T);

				T value{};

				if (buff.active() < size)
					return value;

				std::memcpy((char*)&value, buff.rdata(), size);

				buff.consume(size);

				return value;
			}

			template <repeated_t T, typename V>
			auto from_datas(detail::flex_buffer<V>& buff) -> T
			{
				T value{};

				auto size = from_datas<std::size_t>(buff);

				if (size == 0)
					return value;

				value.resize(size);

				for (std::size_t i = 0; i < size; ++i)
				{
					value[i] = from_datas<typename T::value_type>(buff);
				}

				return value;
			}

			template <string_t T, typename V>
			auto from_datas(detail::flex_buffer<V>& buff) -> T
			{
				auto size = from_datas<std::size_t>(buff);

				T value = size == 0 ? T{} : T(buff.rdata(), size);

				buff.consume(size);

				return value;
			}

			template <reflectable T, typename V>
			auto from_datas(detail::flex_buffer<V>& buff) -> T
			{
				auto from_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ return T{ from_datas<virgo::reflect::tuple_element_t<I, T>>(buff)... }; };

				return from_binary_impl(std::make_index_sequence<virgo::reflect::tuple_size_v<T>>{});
			}
		};
	} // namespace virgo
} // namespace aquarius
