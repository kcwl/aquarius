#pragma once
#include <aquarius/context.hpp>
#include <aquarius/context/generator.hpp>
#include <aquarius/context/invoke.hpp>
#include <aquarius/core/core.hpp>
#include <aquarius/core/elastic.hpp>
#include <aquarius/core/logger.hpp>
#include <aquarius/message/message.hpp>

namespace aquarius
{
	struct message_router
	{
		static bool process(flex_buffer_t& buffer, std::size_t uid)
		{
			auto session_ptr = invoke_session_helper::find(uid);

			uint32_t proto{};

			auto cur = buffer.size();

			if (!elastic::from_binary(proto, buffer))
			{
				XLOG_ERROR() << "parse proto error, maybe message is not complete.";

				return false;
			}

			cur -= buffer.size();

			auto request_ptr = invoke_message_helper::invoke(proto);

			if (!request_ptr)
			{
				request_ptr = std::make_shared<basic_message>();
			}

			auto context_ptr = invoke_context_helper::invoke(proto);

			if (!context_ptr)
				context_ptr = std::make_shared<basic_context>();

			session_ptr->attach(proto, context_ptr);

			context_ptr->on_accept();

			auto result = request_ptr->accept(buffer, context_ptr, session_ptr);

			if (!result)
			{
				buffer.consume(-(static_cast<int>(cur)));

				return result;
			}

			session_ptr->detach(proto);

			return buffer.size() ? process(buffer, uid) : result;
		}
	};
} // namespace aquarius