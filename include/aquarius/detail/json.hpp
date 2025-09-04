#pragma once
#include <boost/json.hpp>
#include <string>

namespace aquarius
{
	namespace detail
	{
		struct json_parse
		{
			template <typename T>
			std::string to_json(const T& v)
			{
				auto value = boost::json::value_from(v);

				return boost::json::serialize(value);
			}

			template <typename T>
			T from_json(const std::string& json)
			{
				auto value = boost::json::parse(json);

				return boost::json::value_to<T>(value);
			}
		};
	} // namespace virgo
} // namespace aquarius
