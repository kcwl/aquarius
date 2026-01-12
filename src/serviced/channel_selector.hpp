#pragma once
#include "channel_schedule.h"
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/router.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace serviced
	{
		constexpr static auto __channel__ = "channel"sv;

		struct channel_tcp_selector
		{
			constexpr static auto tag = proto_tag::tcp;

			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, flex_buffer& buffer)
			{
				co_spawn(session_ptr->get_executor(), mpc_publish(topic, buffer), detached);
			};
		};

		struct channel_http_selector
		{
			constexpr static auto tag = proto_tag::http;

			template <typename Session>
			auto operator()(std::string_view topic, std::shared_ptr<Session> session_ptr, virgo::http_fields,
							flex_buffer& buffer)
			{
				co_spawn(session_ptr->get_executor(), mpc_publish(topic, buffer), detached);
			};
		};
	} // namespace serviced
} // namespace aquarius