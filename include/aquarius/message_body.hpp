#pragma once
#include "detail/parse.hpp"

namespace aquarius
{
	class message_body : public detail::parse
	{
	public:
		message_body() = default;
		virtual ~message_body() = default;
	};
}