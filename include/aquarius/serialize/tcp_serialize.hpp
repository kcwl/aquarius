#pragma once
#include <aquarius/basic_message_serialize.hpp>
#include <aquarius/serialize/binary.hpp>

namespace aquarius
{
	namespace serialize
	{
		class tcp_serialize : public basic_message_serialize<binary_parse>
		{
		public:
			tcp_serialize() = default;
			virtual ~tcp_serialize() = default;
		};
	} // namespace serialize
} // namespace aquarius