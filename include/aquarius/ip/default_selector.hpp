#pragma once
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/virgo/header_field_base.hpp>

namespace aquarius
{
	namespace ip
	{
		struct default_selector
		{
			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr,
							std::shared_ptr<header_field_base> hf, flex_buffer& buffer)
			{
				router<Session>::get_mutable_instance().invoke(topic, session_ptr, hf, buffer);
			};
		};
	} // namespace ip
} // namespace aquarius