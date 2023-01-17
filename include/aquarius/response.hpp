#pragma once
#include <aquarius/impl/field.hpp>
#include <aquarius/impl/header.hpp>
#include <aquarius/impl/message.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class response : public impl::message<impl::tcp_response_header, _Body, Number>
	{
	public:
		response() = default;
		~response() = default;

		DEFINE_VISITABLE(int)
	};

	template <uint32_t Number>
	using null_body_response = response<impl::null_body, Number>;
} // namespace aquarius
