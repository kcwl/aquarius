#pragma once
#include <aquarius/virgo/serialize/basic_message_serialize.hpp>
#include <aquarius/virgo/serialize/string_parse.hpp>
#include <aquarius/virgo/serialize/json.hpp>

namespace aquarius
{
	namespace virgo
	{
		class http_serialize : public basic_message_serialize<string_parse, json_parse>
		{
		public:
			http_serialize() = default;
			virtual ~http_serialize() = default;
		};
	} // namespace virgo
} // namespace aquarius