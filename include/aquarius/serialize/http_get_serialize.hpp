#pragma once
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/get_method.hpp>

namespace aquarius
{
	class http_get_serialize : public basic_serialize<get_parse>
	{
	public:
		http_get_serialize() = default;
		virtual ~http_get_serialize() = default;
	};
} // namespace aquarius