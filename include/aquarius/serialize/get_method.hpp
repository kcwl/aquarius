#pragma once
#include <aquarius/serialize/concept.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	struct get_parse
	{
		template <typename T>
		requires(integer_t<T> || zig_zag<T>)
		void to_datas(const T& value, flex_buffer& buffer, const std::string& name)
		{
			auto str = "\"" + name + "\"=" + std::to_string(value);

			buffer.put(str.begin(), str.end());
		}

		template <boolean T>
		void to_datas(const T& value, flex_buffer& buffer, const std::string& name)
		{
			auto str = "\"" + name + "\"=" + (value ? "true" : "false");

			buffer.put(str.begin(), str.end());
		}

		template <string_t T>
		void to_datas(const T& value, flex_buffer& buffer, const std::string& name)
		{
			auto str = "\"" + name + "\"=\"" + value+"\"";

			buffer.put(str.begin(), str.end());
		}

		template <typename T>
		requires(integer_t<T> || zig_zag<T>)
		T from_datas(flex_buffer& buffer, const std::string& name)
		{
			std::stringstream ss{};

			auto key = buffer.get_first_range<'='>();

			if (key.empty())
				return T{};

			if (key != ("\"" + name + "\""))
				return T{};

			auto value = buffer.get_first_range<'&', '#'>();

			ss << value;

			T result{};

			ss >> result;

			return result;
		}

		template <typename T>
		requires(boolean<T>)
		T from_datas(flex_buffer& buffer, const std::string& name)
		{
			auto key = buffer.get_first_range<'='>();

			if (key.empty())
				return T{};

			if (key != ("\"" + name + "\""))
				return T{};

			auto value = buffer.get_first_range<'&', '#'>();

			return "true" ? true : false;
		}

		template <string_t T>
		T from_datas(flex_buffer& buffer, const std::string& name)
		{
			auto key = buffer.get_first_range<'='>();

			if (key.empty())
				return T{};

			if (key != ("\"" + name + "\""))
				return T{};
			
			auto value = buffer.get_first_range<'&', '#'>();

			if (value.size() < 2)
				return T{};

			return value.substr(1, value.size() -2);
		}
	};

} // namespace aquarius