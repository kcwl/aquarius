#pragma once
#include "proto/field.hpp"
#include "proto/message.hpp"

namespace aquarius
{
	template<typename _Header, typename _Body, uint32_t Number>
	class response : public proto::message<_Header, _Body, Number>
	{
	public:
		response() = default;
		~response() = default;
	};

	template<typename _Header, uint32_t Number>
	using null_body_response = response<_Header, proto::empty_body, Number>;
}

