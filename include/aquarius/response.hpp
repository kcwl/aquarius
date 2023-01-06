#pragma once
#include <aquarius/impl/field.hpp>
#include <aquarius/impl/message.hpp>

namespace aquarius
{
	template <typename _Header, typename _Body, uint32_t Number>
	class response : public impl::message<_Header, _Body, Number>
	{
	public:
		response() = default;
		~response() = default;
	};

	template <typename _Header, uint32_t Number>
	using null_body_response = response<_Header, impl::empty_body, Number>;
} // namespace aquarius
