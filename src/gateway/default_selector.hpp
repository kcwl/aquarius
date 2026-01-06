#pragma once
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>
#include <string_view>
#include <aquarius/virgo/http_fields.hpp>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace gateway
	{
		constexpr static auto __transfer__ = "transfer"sv;

		struct transfer_tcp_selector
		{
			constexpr static auto tag = proto_tag::tcp;

			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, flex_buffer& buffer)
			{
				if (!router<tag, Session>::get_mutable_instance().invoke(topic, session_ptr, buffer))
				{
					router<tag, Session>::get_mutable_instance().invoke(__transfer__, session_ptr, buffer);
				}
			};
		};

		struct transfer_http_selector
		{
			constexpr static auto tag = proto_tag::http;

			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, virgo::http_fields hf,  flex_buffer& buffer)
			{
				if (!router<tag, Session>::get_mutable_instance().invoke(topic, session_ptr, hf, buffer))
				{
					router<tag, Session>::get_mutable_instance().invoke(__transfer__, session_ptr, hf, buffer);
				}
			};
		};
	} // namespace gateway
} // namespace aquarius