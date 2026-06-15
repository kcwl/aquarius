#pragma once
#include <aquarius/concepts.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <boost/pfr.hpp>

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

		template <map_t T>
		void to_datas(const T& value, flex_buffer& buffer)
		{
			to_datas(value.size(), buffer);

			for (const auto& v : value)
			{
				to_datas(v.first, buffer);
				to_datas(v.second, buffer);
			}
		}

		template <fixed_t T>
		void to_datas(const T& v, flex_buffer& buffer)
		{
			auto size = sizeof(v.value);

			buffer.sputn((char*)&v.value, size);
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
			std::remove_cvref_t<T> value{};

			uint8_t temp{};

			// ensure at least one byte available
			if (buff.size() < 1)
			{
				throw std::out_of_range("buffer is not enough space");
			}

			// read first byte
			auto len = static_cast<std::size_t>(buff.sgetn((char*)&temp, sizeof(temp)));
			(void)len;

			value = temp;

			if (value >= 0x80)
			{
				value -= 0x80;

				int8_t temp_bit = 7;

				// read continuation bytes
				for (;;)
				{
					if (buff.size() < 1)
					{
						throw std::out_of_range("buffer is not enough space");
					}

					auto len2 = static_cast<std::size_t>(buff.sgetn((char*)&temp, sizeof(temp)));
					(void)len2;

					if ((temp & 0x80) == 0)
					{
						value += (static_cast<T>(temp) << temp_bit);
						break;
					}

					value += static_cast<T>(temp & 0x7f) << temp_bit;
					temp_bit += 7;
				}
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

			// ensure we have enough data available before attempting to read
			if (buff.size() < value_size)
			{
				throw std::out_of_range("buffer is not enough space");
			}

			// read into a temporary buffer to avoid partial-fill UB
			char tmp[value_size];

			auto len = static_cast<std::size_t>(buff.sgetn(tmp, value_size));

			if (len != value_size)
			{
				throw std::out_of_range("buffer is not enough space");
			}

			std::memcpy(&value, tmp, value_size);

			return value;
		}

		template <repeated_t T>
		auto from_datas(flex_buffer& buff) -> T
		{
			T value{};

			auto size = from_datas<std::size_t>(buff);

			value.resize(size);

			for (std::size_t i = 0; i < size; ++i)
			{
				value[i] = from_datas<typename T::value_type>(buff);
			}

			return value;
		}

		template <map_t T>
		auto from_datas(flex_buffer& buff) -> T
		{
			T value{};

			auto size = from_datas<std::size_t>(buff);

			for (std::size_t i = 0; i < size; ++i)
			{
				auto first = from_datas<typename T::value_type::first_type>(buff);
				auto second = from_datas<typename T::value_type::second_type>(buff);

				// value.insert({ first, second });
				value[first] = second;
			}

			return value;
		}

		template <string_t T>
		auto from_datas(flex_buffer& buff) -> T
		{
			auto size = from_datas<std::size_t>(buff);

			std::string value{};
			value.resize(size);

			buff.sgetn(value.data(), value.size());

			return value;
		}

		template <fixed_t T>
		T from_datas(flex_buffer& buffer)
		{
			T v{};

			auto size = sizeof(typename T::value_type);

			auto len = static_cast<std::size_t>(buffer.sgetn((char*)&v.value, size));

			if (len != size)
			{
				throw std::out_of_range("buffer is not enough space");
			}

			return v;
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
