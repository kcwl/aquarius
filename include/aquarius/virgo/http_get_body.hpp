#pragma once
#include <aquarius/serialize/http_html_serialize.hpp>

namespace aquarius
{
	namespace virgo
	{
		class http_get_body : public http_html_serialize
		{
		public:
			http_get_body() = default;
			virtual ~http_get_body() = default;

			virtual void serialize(aquarius::flex_buffer& buffer) override
			{
				this->parse_to(buffer);
			}

			virtual void deserialize(aquarius::flex_buffer& buffer) override
			{
				this->parse_from(buffer);
			}
		};
	} // namespace virgo
} // namespace aquarius