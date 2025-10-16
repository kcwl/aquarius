#pragma once
#include <aquarius/basic_message_serialize.hpp>
#include <aquarius/serialize/json.hpp>

namespace aquarius
{
	namespace serialize
	{
		class http_serialize : public basic_message_serialize<json_parse>
		{
		public:
			http_serialize() = default;
			virtual ~http_serialize() = default;
		};
	}
} // namespace aquarius