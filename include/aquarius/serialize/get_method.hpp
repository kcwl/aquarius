#pragma once
#include <aquarius/serialize/concept.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{

	struct get_parse
	{
		template <typename T>
		requires(integer_t<T> || zig_zag<T>)
		void to_datas(const T& value, const std::string& name, flex_buffer& buffer)
		{
			auto str = name + "=" + std::to_string(value);

			buffer.put(str.begin(), str.end());
		}

		template <boolean T>
		void to_datas(const T& value, const std::string& name, flex_buffer& buffer)
		{
			auto str = name + "=" + value ? "true" : "false";

			buffer.put(str.begin(), str.end());
		}

		template <string_t T>
		void to_datas(const T& value, const std::string& name, flex_buffer& buffer)
		{
			auto str = name + "=" + value;

			buffer.put(str.begin(), str.end());
		}

		template <typename T>
		requires(integer_t<T> || zig_zag<T>)
		T from_datas(flex_buffer& buffer)
		{
			std::stringstream ss{};

			auto pos = buffer.find_first('&');
			if (pos == flex_buffer::npos)
			{
				return {};
			}

			ss.read(buffer.rdata(), pos - buffer.tellg());

			T result{};

			ss >> result;

			return result;
		}

		template <boolean T>
		T from_datas(flex_buffer& buffer)
		{
			auto pos = buffer.find_first('&');
			if (pos == flex_buffer::npos)
			{
				return {};
			}

			std::string f(buffer.rdata(), pos - buffer.tellg());

			return f == "true";
		}

		template <string_t T>
		T from_datas(flex_buffer& buff)
		{
			auto pos = buffer.find_first('&');
			if (pos == flex_buffer::npos)
			{
				return {};
			}

			std::string f(buffer.rdata(), pos - buffer.tellg());

			return f;
		}
	};
} // namespace aquarius