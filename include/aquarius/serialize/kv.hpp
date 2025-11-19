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
			auto sp = std::span<char>((char*)buffer.data().data(), buffer.data().size());

			auto iter = std::ranges::find_if(sp, [&] (const auto c) { return ((c == args), ...); });

			if (iter == sp.end())
				return {};

			auto len = std::ranges::distance(sp.begin(), iter);

			std::string result((char*)buffer.data().data(), len);

			buffer.consume(len);
				
			return result;
		}
	};

} // namespace aquarius