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
		http_serialize()
			: method_(http_method::post)
		{

		}

	public:
		void set_method(http_method m)
		{
			method_ = m;
		}

		void set_method(int m)
		{
			set_method(static_cast<http_method>(m));
		}

		http_method method() const
		{
			return method_;
		}

	private:
		http_method method_;
	};
} // namespace aquarius