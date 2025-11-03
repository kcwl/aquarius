#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/binary.hpp>

namespace aquarius
{
	class tcp_binary_serialize : public basic_serialize<binary_parse>
	{
	public:
		tcp_binary_serialize() = default;
		virtual ~tcp_binary_serialize() = default;
	};
} // namespace aquarius