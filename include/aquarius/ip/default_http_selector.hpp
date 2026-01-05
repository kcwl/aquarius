#pragma once
#include <aquarius/ip/protocol.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <aquarius/ip/router.hpp>

namespace aquarius
{
	namespace ip
	{
		struct default_http_selector
		{
			constexpr static auto tag = proto_tag::http;
			template <typename Session>
			void operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, virgo::http_fields hf,
							flex_buffer& buffer)
			{
				router<tag, Session>::get_mutable_instance().invoke(topic, session_ptr, hf, buffer);
			}
		};
	} // namespace ip
} // namespace aquarius