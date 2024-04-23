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

			if (!session_ptr)
				return false;

			auto pos = buffer.pubseekoff(0,std::ios::cur,std::ios::out);

			pack_flag flag{};

			if (!elastic::from_binary(flag, buffer))
			{
				XLOG_ERROR() << "parse proto error, maybe message is not haved flag.";

				return false;
			}

			uint32_t proto{};
			if (!elastic::from_binary(proto, buffer))
			{
				XLOG_ERROR() << "parse proto error, maybe message is not complete.";

				return false;
			}

			bool result = true;

			switch (flag)
			{
			case pack_flag::normal:
				{
					result = normalize(buffer, session_ptr, proto, uid);
				}
				break;
			case pack_flag::middle:
				{
					session_ptr->attach_buffer(proto, buffer);
				}
				break;
			case pack_flag::end:
				{
					auto buf = session_ptr->complete(proto, buffer);

					result = normalize(buf, session_ptr, proto, uid);
				}
				break;
			default:
				break;
			}

			if (!result)
				buffer.pubseekpos(pos, std::ios::out);

			return result;
		}

	protected:
		static bool normalize(flex_buffer_t& buffer, std::shared_ptr<basic_session> session_ptr,
							  const std::size_t proto, const std::size_t uid)
		{
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

			if (result)
				session_ptr->detach(proto);

			return buffer.size() ? process(buffer, uid) : result;
		}
	};
} // namespace aquarius