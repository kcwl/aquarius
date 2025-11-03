#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/kv.hpp>

namespace aquarius
{
	class http_kv_serialize : public basic_serialize<kv_parse>
	{
	public:
		http_kv_serialize() = default;
		virtual ~http_kv_serialize() = default;
	};
} // namespace aquarius