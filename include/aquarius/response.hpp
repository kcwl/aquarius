#pragma once
#include "message/field.hpp"
#include "message/message.hpp"

namespace aquarius
{
	template<typename _Header, typename _Body, uint32_t Number>
	class response : public message::basic_message<false, _Header, _Body, Number>
	{
	public:
		response() = default;
		~response() = default;
	};

	template<typename _Header, uint32_t Number>
	using null_body_response = response<_Header, message::empty_body, Number>;
}

