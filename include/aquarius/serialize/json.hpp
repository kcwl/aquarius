#pragma once
#include <boost/json.hpp>
#include <string>
#include <aquarius/serialize/flex_buffer.hpp>
#include <string>

namespace aquarius
{
	namespace json =  boost::json;

	namespace serialize
	{
		struct json_parse
		{
			template <typename T, typename U>
			void to_datas(const T& v, flex_buffer<U>& buffer)
			{
				auto value = boost::json::value_from(v);

				auto str = boost::json::serialize(value);

				if (str == "null")
					return;

				buffer.put(str.begin(), str.end());
			}

			template <typename T, typename U>
			T from_datas(flex_buffer<U>& buffer)
			{
				if (buffer.empty())
					return {};

				std::string json;

				buffer.get(json);

				auto value = boost::json::parse(json);

				return boost::json::value_to<T>(value);
			}
		};

		inline std::vector<char> json_value_to_array(const boost::json::value& v)
		{
			std::vector<char> result{};

			const auto& arr = v.try_as_array();

			if (arr.has_error())
				return result;

			auto& arr_val = arr.value();

			std::transform(arr_val.begin(), arr_val.end(), result.begin(), [] (const auto& v)
						   {
							   return static_cast<char>(*v.try_as_int64());
						   });

			return result;
		}

		inline boost::json::value json_value_from_array(const std::vector<char>& vec)
		{
			boost::json::value v{};
			auto& arr = v.emplace_array();

			std::copy(vec.begin(), vec.end(), std::back_inserter(arr));

			return v;
		}

		template<typename T>
		inline boost::json::value json_value_from_object(T t)
		{
			boost::json::value v{};
			auto& obj = v.emplace_object();

			boost::json::value_from<T>(std::move(t), obj);

			return v;
		}
	} // namespace virgo
} // namespace aquarius
