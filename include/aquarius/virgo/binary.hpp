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
			template <integer_t T, typename BufferSequence>
			void to_binary(const T& value, BufferSequence& buff)
			{
				auto temp = value;

				while (temp >= 0x80)
				{
					buff.push_back(static_cast<char>(temp | 0x80));

					temp >>= 7;
				}
				buff.push_back(static_cast<char>(temp));
			}

			template <zig_zag T, typename BufferSequence>
			void to_binary(const T& value, BufferSequence& buff)
			{
				auto temp = (value << 1) ^ (value >> (sizeof(T) * 8 - 1));

				return to_binary<uint64_t>(temp, buff);
			}

			template <pod_t T, typename BufferSequence>
			void to_binary(T value, BufferSequence& buff)
			{
				constexpr auto size = sizeof(T);

				std::copy((char*)&value, (char*)&value + size, std::back_inserter(buff));
			}

			template <repeated_t T, typename BufferSequence>
			void to_binary(const T& value, BufferSequence& buff)
			{
				to_binary(value.size(), buff);

				for (auto& c : value)
				{
					to_binary(c, buff);
				}
			}

			template <string_t T, typename BufferSequence>
			void to_binary(const T& value, BufferSequence& buff)
			{
				to_binary(value.size(), buff);

				std::copy(std::begin(value), std::end(value), std::back_inserter(buff));
			}

			template <reflectable T, typename BufferSequence>
			void to_binary(const T& value, BufferSequence& buff)
			{
				auto to_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ (to_binary(reflect::get<I, T>(value), buff), ...); };

				to_binary_impl(std::make_index_sequence<reflect::tuple_size_v<T>>{});
			}

			template <integer_t T, typename BufferSequence>
			auto from_binary(BufferSequence& buff) -> T
			{
				auto span_buff = std::span(buff);

				auto iter =
					std::find_if(span_buff.begin(), span_buff.end(), [](const auto s) { return (s & 0x80) == 0; });

				if (iter == span_buff.end())
				{
					throw std::runtime_error("syntax error!");
				}

				auto length = std::distance(span_buff.begin(), iter) + 1;

				T value = static_cast<uint8_t>(span_buff[0]);
				if (value >= 0x80)
				{
					value -= 0x80;

					auto sub_span = span_buff.subspan(1, length - 1);

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

				auto dynamic_buf = boost::asio::dynamic_buffer(buff);

				dynamic_buf.consume(length);

				return value;
			}

			template <zig_zag T, typename BuffSequence>
			auto from_binary(BuffSequence& buff) -> T
			{
				T value = static_cast<T>(from_binary<uint64_t>(buff));

				return static_cast<T>((value >> 1) ^ (~(value & 1) + 1));
			}

			template <pod_t T, typename BufferSequence>
			auto from_binary(BufferSequence& buff)
			{
				constexpr auto size = sizeof(T);

				T value{};

				if (buff.size() < size)
					return value;

				std::memcpy((char*)&value, buff.data(), size);

				auto dynamic_buf = boost::asio::dynamic_buffer(buff);

				dynamic_buf.consume(size);

				return value;
			}

			template <repeated_t T, typename BuffSequence>
			auto from_binary(BuffSequence& buff) -> T
			{
				T value{};
				std::size_t size = from_binary<std::size_t>(buff);

				if (size == 0)
					return value;

				value.resize(size);

				for (int i = 0; i < size; ++i)
				{
					value[i] = from_binary<typename T::value_type>(buff);
				}

				return value;
			}

			template <string_t T, typename BuffSequence>
			auto from_binary(BuffSequence& buff) -> T
			{
				using value_type = typename BuffSequence::value_type;

				std::size_t size = from_binary<std::size_t>(buff);

				T value = T(buff.data(), size);

				auto dynamic_buf = boost::asio::dynamic_buffer(buff);

				dynamic_buf.consume(size);

				return value;
			}

			template <reflectable T, typename BuffSequence>
			auto from_binary(BuffSequence& buff) -> T
			{
				auto from_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
				{ return T{ from_binary<reflect::tuple_element_t<I, T>>(buff)... }; };

				return from_binary_impl(std::make_index_sequence<reflect::tuple_size_v<T>>{});
			}
		};
	} // namespace virgo
} // namespace aquarius
