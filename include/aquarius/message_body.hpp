#pragma once
#include "parse.hpp"


namespace aquarius
{
	class message_body : public parse
	{
	public:
		message_body() = default;
		virtual ~message_body() = default;
	};
}