#pragma once
#include <aquarius/impl/message.hpp>

namespace aquarius
{
	template <typename _Header, typename _Body, uint32_t Number>
	class request : public impl::message<_Header, _Body, Number>
	{
	public:
		request() = default;
		~request() = default;

		DEFINE_VISITOR()
	};

	template <typename _Header, uint32_t Number>
	using null_body_request = request<_Header, void, Number>;
} // namespace aquarius