#pragma once
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	class session_base
	{
	public:
		virtual boost::asio::awaitable<void> send_packet(std::size_t proto, flex_buffer_t) = 0;
	};
} // namespace aquarius