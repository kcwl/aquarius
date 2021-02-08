#pragma once
#include "message.hpp"
#include "header.hpp"

namespace aquarius
{
	namespace net
	{
		template<class Body, std::size_t Number>
		class request : public message<proto_header, Body, Number>
		{
		public:
			request() = default;
			virtual ~request() = default;

		public:

		};
	}
}