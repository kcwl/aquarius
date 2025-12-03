#pragma once
#include <boost/json.hpp>
#include <string>
#include <aquarius/serialize/flex_buffer.hpp>
#include <string>

namespace aquarius
{
	namespace json = boost::json;

	struct json_parse
	{
		template <typename T>
		void to_datas(const T& v, flex_buffer& buffer)
		{
			auto value = boost::json::value_from(v);

			auto str = boost::json::serialize(value);

			if (str == "null")
				return;

			buffer.sputn(str.c_str(), str.size());
		}

		template <typename T>
		T from_datas(flex_buffer& buffer)
		{
			std::string json;
			json.resize(buffer.size());

			buffer.sgetn(json.data(),json.size());

			if (json.empty())
				return T{};

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

		result.resize(arr_val.size());

		std::transform(arr_val.begin(), arr_val.end(), result.begin(),
					   [](const auto& v) { return static_cast<char>(*v.try_as_int64()); });

		return result;
	}

	inline boost::json::value json_value_from_array(const std::vector<char>& vec)
	{
		boost::json::value v{};
		auto& arr = v.emplace_array();

		std::copy(vec.begin(), vec.end(), std::back_inserter(arr));

		return v;
	}

	template <typename T>
	inline boost::json::value json_value_from_object(const T& t)
	{
		auto value = t;

		return boost::json::value_from<T>(std::move(value));
	}
} // namespace aquarius
