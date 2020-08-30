#pragma once
#include "message.hpp"
#include "header.hpp"

namespace aquarius
{
	namespace aqnet
	{
		template<class Body, std::size_t Number>
		class tcp_response : public response<tcp_response_header, Body, Number>
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
		class http_response : public response<http_response_header, Body, Number>
		{

		};
	}
}

