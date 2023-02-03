#pragma once
#include <aquarius/tcp/field.hpp>
#include <aquarius/tcp/header.hpp>
#include <aquarius/tcp/message.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class response : public tcp::message<tcp::tcp_response_header, _Body, Number>
	{
	public:
		response() = default;
		~response() = default;

		DEFINE_VISITABLE(int)
	};

	template <uint32_t Number>
	using null_body_response = response<tcp::null_body, Number>;
} // namespace aquarius
