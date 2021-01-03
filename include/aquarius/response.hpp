#pragma once
#include "detail\message.hpp"
#include "detail\header.hpp"

namespace aquarius
{
	namespace aqnet
	{
		template<class Body, std::size_t Number>
		class tcp_response : public response<Body, Number>
		{
		public:
			virtual int parse_bytes(const void*, std::size_t)
			{
				return 0;
			}

			virtual void* to_bytes() override
			{
				return this->body().to_bytes();
			}
		};

		template<class Body, std::size_t Number>
		class http_response : public response<Body, Number>
		{

		};
	}
}

