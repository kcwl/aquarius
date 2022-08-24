#pragma once
#include "message/message.hpp"

namespace aquarius
{
	template<typename _Header, typename _Body, std::size_t Number>
	class request : public message::basic_message<true, _Header, _Body, Number>
	{
	public:
		request() = default;
		~request() = default;
	};
}