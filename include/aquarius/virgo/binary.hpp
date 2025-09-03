#pragma once
#include <span>
#include <aquarius/virgo/concepts.hpp>
#include <aquarius/virgo/reflection.hpp>
#include <boost/asio/buffer.hpp>

namespace aquarius
{
	namespace virgo
	{
		struct binary_parse
		{
			template <integer_t T, typename V>
			void to_datas(const T& value, std::vector<V>& buff)
			{
				auto temp = value;

				while (temp >= 0x80)
				{
					buff.push_back(static_cast<char>(temp | 0x80));

					temp >>= 7;
				}
				buff.push_back(static_cast<char>(temp));
			}

			template <zig_zag T, typename V>
			void to_datas(const T& value, std::vector<V>& buff)
			{
				auto temp = (value << 1) ^ (value >> (sizeof(T) * 8 - 1));

				return to_datas<uint64_t>(temp, buff);
			}

			template <pod_t T, typename V>
			void to_datas(T value, std::vector<V>& buff)
			{
				constexpr auto size = sizeof(T);

				std::copy((char*)&value, (char*)&value + size, std::back_inserter(buff));
			}

			template <repeated_t T, typename V>
			void to_datas(const T& value, std::vector<V>& buff)
			{
				to_datas(value.size(), buff);

				for (auto& c : value)
				{
					to_datas(c, buff);
				}
			}

			template <string_t T, typename V>
			void to_datas(const T& value, std::vector<V>& buff)
			{
				to_datas(value.size(), buff);

				std::copy(std::begin(value), std::end(value), std::back_inserter(buff));
			}

			template <reflectable T, typename V>
			void to_datas(const T& value, std::vector<V>& buff)
			{
				auto to_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ (to_datas(reflect::get<I, T>(value), buff), ...); };

				to_binary_impl(std::make_index_sequence<reflect::tuple_size_v<T>>{});
			}

			template <integer_t T, typename V>
			auto from_datas(std::span<V> buff, std::size_t& consume) -> T
			{
				auto current_buffer = buff.subspan(consume);

				auto iter = std::find_if(current_buffer.begin(), current_buffer.end(),
										 [](const auto s) { return (s & 0x80) == 0; });

				if (iter == current_buffer.end())
				{
					throw std::runtime_error("syntax error!");
				}

				consume += std::distance(current_buffer.begin(), iter) + 1;

				T value = static_cast<uint8_t>(current_buffer[0]);
				if (value >= 0x80)
				{
					value -= 0x80;

					auto sub_span = current_buffer.subspan(1, consume - 1);

					int8_t temp_bit = 7;

					for (auto& s : sub_span)
					{
						if ((s & 0x80) != 0)
						{
							value += static_cast<T>(s & 0x7f) << temp_bit;

							temp_bit += 7;
						}
						else
						{
							value += (static_cast<T>(s) << temp_bit);
						}
					}
				}

				return value;
			}

			template <zig_zag T, typename V>
			auto from_datas(std::vector<V> buff, std::size_t& consume) -> T
			{
				return static_cast<T>(from_datas<uint64_t>(buff, consume));
			}

			template <pod_t T, typename V>
			auto from_datas(std::span<V> buff, std::size_t& consume) -> T
			{
				auto current_buffer = buff.subspan(consume);

				constexpr auto size = sizeof(T);

				T value{};

				if (current_buffer.size() < size)
					return value;

				consume += size;

				std::memcpy((char*)&value, current_buffer.data(), size);

				return value;
			}

			template <repeated_t T, typename V>
			auto from_datas(std::span<V> buff, std::size_t& consume) -> T
			{
				T value{};
				std::size_t total = 0;

				auto size = from_datas<std::size_t>(buff, consume);

				if (size == 0)
					return value;

				value.resize(size);

				for (std::size_t i = 0; i < size; ++i)
				{
					value[i] = from_datas<typename T::value_type>(buff, consume);
				}

				return value;
			}

			template <string_t T, typename V>
			auto from_datas(std::vector<V> buff, std::size_t& consume) -> T
			{
				auto size = from_datas<std::size_t>(buff, consume);

				T value = size == 0 ? T{} : T(buff.subspan(consume).data(), size);

				consume += size;

				return value;
			}

			template <reflectable T, typename V>
			auto from_datas(std::vector<V> buff, std::size_t& consume) -> T
			{
				auto from_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ return T{ from_datas<reflect::tuple_element_t<I, T>>(buff, consume)... }; };

				return from_binary_impl(std::make_index_sequence<reflect::tuple_size_v<T>>{});
			}
		};
	} // namespace virgo
} // namespace aquarius
