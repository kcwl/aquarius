#pragma once
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace gateway
	{
		constexpr static auto __transfer__ = "transfer"sv;

		struct transfer_selector
		{
			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr,
							std::shared_ptr<header_field_base> hf, flex_buffer& buffer)
			{
				if (!router<Session>::get_mutable_instance().invoke(topic, session_ptr, hf, buffer))
				{
					router<Session>::get_mutable_instance().invoke(__transfer__, session_ptr, hf, buffer);
				}
			};
		};
	} // namespace gateway
} // namespace aquarius