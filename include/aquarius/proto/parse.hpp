#pragma once
#include "../io.hpp"

namespace aquarius
{
	namespace proto
	{
		template<typename _Stream = flex_buffer_t>
		class parse
		{
		public:
			using stream_type = _Stream;

		public:
			virtual bool parse_bytes(stream_type& archive) = 0;

			virtual bool to_bytes(stream_type& archive) = 0;
		};
	}

	using ft_parse = proto::parse<>;
}