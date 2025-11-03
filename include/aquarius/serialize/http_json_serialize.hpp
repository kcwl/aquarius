#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/json.hpp>

namespace aquarius
{
	class http_json_serialize : public basic_serialize<json_parse>
	{
	public:
		http_json_serialize() = default;
		virtual ~http_json_serialize() = default;
	};
} // namespace aquarius