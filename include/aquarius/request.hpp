#pragma once
#include <aquarius/impl/header.hpp>
#include <aquarius/impl/message.hpp>

namespace aquarius
{
	template <typename _Body, uint32_t Number>
	class request : public impl::message<impl::tcp_request_header, _Body, Number>
	{
	public:
		request() = default;
		~request() = default;

		DEFINE_VISITABLE(int)
	};

	template <uint32_t Number>
	using null_body_request = request<void, Number>;
} // namespace aquarius