#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/concepts.hpp>
#include <aquarius/detail/member_pointer.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>
#include <aquarius/virgo/http_method.hpp>

namespace aquarius
{
	class http_serialize : public basic_serialize<json_parse>, public basic_serialize<kv_parse>
	{
	public:
		using json_base = basic_serialize<json_parse>;
		using kv_base = basic_serialize<kv_parse>;

	public:
		template <typename T, auto Ptr = nullptr, auto Method = http_method::post>
		void parse_to(T&& value, flex_buffer& buffer)
		{
			if constexpr (Method == http_method::get)
			{
				constexpr auto col_name = member_pointer_name<T, Ptr>::value;

				return kv_base::parse_.to_datas(std::forward<T>(value), buffer, col_name);
			}
			else
			{
				return json_base::parse_.to_datas(std::forward<T>(value), buffer);
			}
		}

		template <typename T, auto Ptr = nullptr, auto Method = http_method::post>
		auto parse_from(flex_buffer& buffer) -> T
		{
			if constexpr (Method == http_method::get)
			{
				constexpr auto col_name = member_pointer_name<T, Ptr>::value;
				return kv_base::parse_.from_datas<T>(buffer, col_name);
			}
			else
			{
				return json_base::parse_.from_datas<T>(buffer);
			}
		}
	};
} // namespace aquarius