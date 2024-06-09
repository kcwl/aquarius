#pragma once
#include <aquarius/message/custom.hpp>
#include <aquarius/message/message.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class request : public message<custom_tcp_request_header, _Body, Number>,
					public std::enable_shared_from_this<request<_Body, Number>>
	{
	public:
		AQUARIUS_VISITABLE_MESSAGE()
	};
} // namespace aquarius