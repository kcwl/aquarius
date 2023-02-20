#pragma once
#include <aquarius/tcp/header.hpp>
#include <aquarius/tcp/message.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class request : public tcp::message<tcp::tcp_request_header, _Body, Number>
	{
	public:
		request() = default;
		~request() = default;

		DEFINE_VISITABLE(int)
	};

	template <uint32_t Number>
	using null_body_request = request<tcp::null_body, Number>;
} // namespace aquarius