#pragma once
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>

namespace aquarius
{
	namespace gateway
	{
		struct tcp_transfer_selector
		{
			constexpr static auto tag = proto_tag::tcp;

			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, flex_buffer& buffer)
			{
				router<tag, Session>::get_mutable_instance().invoke(__transfer__, session_ptr, buffer);
			};
		};
	} // namespace gateway
} // namespace aquarius