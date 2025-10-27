#pragma once
#include <aquarius/concepts/concepts.hpp>
#include <boost/pfr.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{

	struct binary_parse
	{
		template <integer_t T>
		void to_datas(const T& value, flex_buffer& buff)
		{
			auto temp = value;

			while (temp >= 0x80)
			{
				buff.put(static_cast<uint8_t>(temp | 0x80));

				temp >>= 7;
			}
			buff.put(static_cast<uint8_t>(temp));
		}

		template <zig_zag T>
		void to_datas(const T& value, flex_buffer& buff)
		{
			uint64_t temp = (value << 1) ^ (value >> (sizeof(T) * 8 - 1));

			return to_datas<uint64_t>(temp, buff);
		}

		template <pod_t T>
		void to_datas(T value, flex_buffer& buff)
		{
			constexpr auto size = sizeof(T);

			buff.put((uint8_t*)&value, size);
		}

		template <repeated_t T>
		void to_datas(const T& value, flex_buffer& buff)
		{
			to_datas(value.size(), buff);

			for (auto& c : value)
			{
				to_datas(c, buff);
			}
		}

		template <string_t T>
		void to_datas(const T& value, flex_buffer& buff)
		{
			to_datas(value.size(), buff);

			buff.put(std::begin(value), std::end(value));
		}

		template <reflectable T>
		void to_datas(const T& value, flex_buffer& buff)
		{
			auto to_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
			{ (to_datas(boost::pfr::get<I, T>(value), buff), ...); };

			to_binary_impl(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
		}

		template <integer_t T>
		auto from_datas(flex_buffer& buff) -> T
		{
			T value = static_cast<uint8_t>(buff.get());
			if (value >= 0x80)
			{
				value -= 0x80;

				int8_t temp_bit = 7;

				while (static_cast<uint8_t>(buff.peek()) > 0x7f)
				{
					auto s = static_cast<uint8_t>(buff.get());

					value += static_cast<T>(s & 0x7f) << temp_bit;

					temp_bit += 7;
				}

				value += (static_cast<T>(buff.get()) << temp_bit);
			}

			return value;
		}

		template <zig_zag T>
		auto from_datas(flex_buffer& buff) -> T
		{
			uint64_t temp = from_datas<uint64_t>(buff);

			return static_cast<T>((temp >> 1) ^ (-static_cast<int64_t>(temp & 0x1)));
		}

		template <pod_t T>
		auto from_datas(flex_buffer& buff) -> T
		{
			return buff.template get<T>();
		}

		template <repeated_t T>
		auto from_datas(flex_buffer& buff) -> T
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

		template <string_t T>
		auto from_datas(flex_buffer& buff) -> T
		{
			auto size = from_datas<std::size_t>(buff);

			T value = size == 0 ? T{} : T((char*)buff.rdata(), size);

			buff.consume(size);

			return value;
		}

		template <reflectable T>
		auto from_datas(flex_buffer& buff) -> T
		{
			auto from_binary_impl = [&]<std::size_t... I>(std::index_sequence<I...>)
			{ return T{ from_datas<boost::pfr::tuple_element_t<I, T>>(buff)... }; };

			return from_binary_impl(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
		}
	};

} // namespace aquarius
