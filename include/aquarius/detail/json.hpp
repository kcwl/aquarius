#pragma once
#include <boost/json.hpp>
#include <string>
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	namespace detail
	{
		struct json_parse
		{
			template <typename T, typename U>
			void to_datas(const T& v, detail::flex_buffer<U>& buffer)
			{
				auto value = boost::json::value_from(v);

				auto str = boost::json::serialize(value);

				if (str == "null")
					return;

				if (str.size() > buffer.remain())
					return;

				std::copy(str.begin(), str.end(), buffer.wdata());

				buffer.commit(str.size());
			}

			template <typename T>
			T from_datas(detail::flex_buffer<T>& buffer)
			{
				std::string json(buffer.rdata(), buffer.active());

				buffer.consume(json.size());

				auto value = boost::json::parse(json);

				return boost::json::value_to<T>(value);
			}
		};
	} // namespace virgo
} // namespace aquarius
