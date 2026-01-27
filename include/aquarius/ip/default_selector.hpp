#pragma once
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/virgo/header_fields.hpp>

namespace aquarius
{
	namespace ip
	{
		struct default_selector
		{
			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, virgo::header_fields& hf,
							flex_buffer& buffer)
			{
				router<Session>::get_mutable_instance().invoke(topic, session_ptr, hf, buffer);
			};
		};
	} // namespace ip
} // namespace aquarius