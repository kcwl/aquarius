#pragma once
#include <aquarius/message/message.hpp>
#include <aquarius/message/custom.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class response : public message<custom_tcp_response_header, _Body, Number>,
					 public std::enable_shared_from_this<response<_Body, Number>>
	{
	public:
		AQUARIUS_VISITABLE_MESSAGE()
	};
} // namespace aquarius
