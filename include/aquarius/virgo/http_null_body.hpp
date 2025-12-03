#pragma once
#include <aquarius/serialize/http_json_serialize.hpp>

namespace aquarius
{
	namespace virgo
	{
		class http_null_body : public http_json_serialize
		{
		public:
			http_null_body() = default;
			virtual ~http_null_body() = default;

			http_null_body(http_null_body&&) = default;

			http_null_body& operator=(http_null_body&&) = default;

			virtual void serialize(aquarius::flex_buffer&) override
			{
				return;
			}

			virtual void deserialize(aquarius::flex_buffer&) override
			{
				return;
			}
		};
	} // namespace virgo
} // namespace aquarius