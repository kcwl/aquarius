#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/json.hpp>

namespace aquarius
{
	class http_post_serialize : public basic_serialize<json_parse>
	{
	public:
		http_post_serialize() = default;
		virtual ~http_post_serialize() = default;
	};
} // namespace aquarius