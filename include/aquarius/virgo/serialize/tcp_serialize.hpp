#pragma once
#include <aquarius/virgo/serialize/basic_message_serialize.hpp>
#include <aquarius/virgo/serialize/binary.hpp>

namespace aquarius
{
	namespace virgo
	{
		class tcp_serialize : public basic_message_serialize<binary_parse>
		{
		public:
			tcp_serialize() = default;
			virtual ~tcp_serialize() = default;
		};
	} // namespace virgo
} // namespace aquarius