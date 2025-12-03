#pragma once
#include <aquarius/serialize/concept.hpp>
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
				buff.sputc(static_cast<char>(temp | 0x80));

				temp >>= 7;
			}
			buff.sputc(static_cast<char>(temp));
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

			buff.sputn((char*)&value, size);
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

			buff.sputn(value.data(), value.size());
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
			T value{};

			uint8_t temp{};
			buff.sgetn((char*)&temp, sizeof(temp));

			value = temp;

			if (value == static_cast<T>(flex_buffer::traits_type::eof()))
				throw serialize_error::not_enough_space;

			if (value >= 0x80)
			{
				value -= 0x80;

				int8_t temp_bit = 7;

				buff.sgetn((char*)&temp, sizeof(temp));

				while (temp > 0x7f)
				{
					value += static_cast<T>(temp & 0x7f) << temp_bit;

					temp_bit += 7;
					buff.sgetn((char*)&temp, sizeof(temp));
				}

				if (temp == flex_buffer::traits_type::eof())
					throw serialize_error::not_enough_space;

				value += (static_cast<T>(temp) << temp_bit);
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
			T value{};

			constexpr auto value_size = sizeof(T);

			auto len = buff.sgetn((char*)&value, value_size);

			if (len != value_size)
				throw serialize_error::not_enough_space;

			return value;
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

			std::string value{};
			value.resize(size);

			std::size_t sz = buff.sgetn(value.data(), value.size());

			if (sz != size)
				throw serialize_error::not_enough_space;

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
