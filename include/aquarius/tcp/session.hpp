#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/tcp/processor.hpp>
#include <aquarius/ip/tcp.hpp>

namespace aquarius
{
	namespace tcp
	{
		using client_session = basic_session<false, ip::tcp, processor<ip::tcp>>;

		using server_session = basic_session<true, ip::tcp, processor<ip::tcp>>;
	} // namespace tcp
} // namespace aquarius