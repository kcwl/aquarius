#pragma once
#include "proto/message.hpp"

namespace aquarius
{
	template<typename _Header, typename _Body, uint32_t Number>
	class request : public proto::message<_Header, _Body, Number>
	{
	public:
		request() = default;
		~request() = default;
	};

	template<typename _Header, uint32_t Number>
	using null_body_request = request<_Header, proto::empty_body, Number>;
}