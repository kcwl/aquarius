#pragma once
#include <aquarius/elastic.hpp>
#include <aquarius/defines.hpp>
#include <aquarius/resolver.hpp>
#include <aquarius/invoke/invoke_message.hpp>
#include <aquarius/invoke/invoke_context.hpp>
#include <aquarius/session/session_manager.hpp>

namespace aquarius
{
	struct session_iovoke_helper
	{
		static read_handle_result invoke(flex_buffer_t& buffer, std::size_t uid)
		{
			auto session_ptr = find_session(uid);

			if (!session_ptr)
				return read_handle_result::unknown_error;

			uint32_t proto{};

			std::size_t total{};

			auto result = resolver<tcp>::template from_binay(buffer, proto, total);

			if (result != read_handle_result::ok)
				return result;

			auto request_ptr = message_invoke_helper::invoke(proto);

			auto context_ptr = context_invoke_helper::invoke(proto);

			if (!request_ptr)
			{
				buffer.consume(total);

				request_ptr = std::make_shared<basic_message>();
			}

			if (!context_ptr)
				context_ptr = std::make_shared<basic_context>();

			session_ptr->attach(proto, context_ptr);

			session_ptr->on_accept();

			result = request_ptr->accept(buffer, context_ptr, session_ptr);

			if (result == read_handle_result::ok)
				session_ptr->detach(proto);

			return result;
		}
	};

	struct timer_invoke_helper
	{
		static void invoke()
		{
			session_manager::instance().timeout();
		}
	};

	struct broadcast_invoke_helper
	{
		template<typename _Response>
		static void invoke(_Response&& resp)
		{
			flex_buffer_t fs{};
			resp.to_binary(fs);

			session_manager::instance().broadcast(std::move(fs));
		}
	};
}