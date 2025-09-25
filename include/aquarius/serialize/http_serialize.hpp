#pragma once
#include <aquarius/serialize/basic_message_serialize.hpp>
#include <aquarius/serialize/json.hpp>

namespace aquarius
{
	namespace virgo
	{
		class http_serialize : public basic_message_serialize<json_parse>
		{
		public:
			http_serialize() = default;
			virtual ~http_serialize() = default;
		};
	} // namespace virgo
} // namespace aquarius