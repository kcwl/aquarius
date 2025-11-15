#pragma once
#include <aquarius/serialize/concept.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	struct kv_parse
	{
		template <typename T>
		requires(integer_t<T> || zig_zag<T>)
		void to_datas(const T& value, flex_buffer& buffer, const std::string& name)
		{
			auto str = name + "=" + std::to_string(value);

			buffer.sputn(str.c_str(), str.size());
		}

		template <boolean T>
		void to_datas(const T& value, flex_buffer& buffer, const std::string& name)
		{
			auto str = name + "=" + (value ? "true" : "false");

			buffer.sputn(str.c_str(), str.size());
		}

		template <string_t T>
		void to_datas(const T& value, flex_buffer& buffer, const std::string& name)
		{
			auto str = name + "=" + value;

			buffer.sputn(str.c_str(), str.size());
		}

		template <typename T>
		requires(integer_t<T> || zig_zag<T>)
		T from_datas(flex_buffer& buffer, const std::string& name)
		{
			std::stringstream ss{};

			auto key = get_first_range<'='>(buffer);

			if (key.empty())
				return T{};

			if (key != name)
				return T{};

			auto value = get_first_range<'&', '#'>(buffer);

			ss << value;

			T result{};

			ss >> result;

			return result;
		}

		template <typename T>
		requires(boolean<T>)
		T from_datas(flex_buffer& buffer, const std::string& name)
		{
			auto key = get_first_range<'='>(buffer);

			if (key.empty())
				return T{};

			if (key != name)
				return T{};

			auto value = get_first_range<'&', '#'>(buffer);

			return "true" ? true : false;
		}

		template <string_t T>
		T from_datas(flex_buffer& buffer, const std::string& name)
		{
			auto key = get_first_range<'='>(buffer);

			if (key.empty())
				return T{};

			if (key != name)
				return T{};
			
			auto value = get_first_range<'&', '#'>(buffer);

			if (value.size() < 2)
				return T{};

			return value;
		}

	private:
		template<char... args>
		std::string get_first_range(flex_buffer& buffer)
		{
			auto start_pos = buffer.data().data();

			auto c = buffer.sgetc();

			while (buffer.size() != 0)
			{
				if (c == flex_buffer::traits_type::eof())
					return {};

				if (((c == args) || ...))
					return std::string(buffer.data().data(), buffer.data().size());
			}

			return {};
		}
	};

} // namespace aquarius