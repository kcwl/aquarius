#pragma once
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>

namespace aquarius
{
	namespace ip
	{
		struct default_tcp_selector
		{
			constexpr static auto tag = proto_tag::tcp;

			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, flex_buffer& buffer)
			{
				router<tag, Session>::get_mutable_instance().invoke(topic, session_ptr, buffer);
			};
		};
	} // namespace ip
} // namespace aquarius