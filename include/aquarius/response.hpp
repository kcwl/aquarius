#pragma once
#include "message.hpp"
#include "header.hpp"

namespace aquarius
{
	namespace net
	{
		template<class Body, std::size_t Number>
		class response : public message<proto_header, Body, Number>
		{
		public:
			response() = default;
			virtual ~response() = default;

		public:

		};
	}
}

