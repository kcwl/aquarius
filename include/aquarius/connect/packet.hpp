#pragma once
#include <aquarius/context.hpp>
#include <aquarius/context/generator.hpp>
#include <aquarius/context/invoke.hpp>
#include <aquarius/core/elastic.hpp>
#include <aquarius/core/error_code.hpp>
#include <aquarius/core/logger.hpp>
#include <aquarius/message/generator.hpp>
#include <aquarius/message/message.hpp>

namespace aquarius
{
	class packet
	{
	public:
		static error_code process(flex_buffer_t& buffer, std::size_t uid)
		{
			aquarius::error_code ec{};

			auto session_ptr = invoke_session_helper::find(uid);

			if (!session_ptr)
				return ec = error_code_result::nosession;

			auto pos = buffer.pubseekoff(0, std::ios::cur, std::ios::out);

			pack_flag flag{};

			if (!elastic::from_binary(flag, buffer))
			{
				XLOG_WARNING() << "parse proto error, maybe message is not haved flag.";

				return ec = error_code_result::incomplete;
			}

			uint32_t proto{};
			if (!elastic::from_binary(proto, buffer))
			{
				XLOG_WARNING() << "parse proto error, maybe message is not complete.";

				return ec = error_code_result::unknown;
			}

			switch (flag)
			{
			case pack_flag::normal:
				{
					ec = normalize(buffer, session_ptr, proto, uid);
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

					ec = normalize(buf, session_ptr, proto, uid);
				}
				break;
			default:
				break;
			}

			if (ec.value() != static_cast<int>(error_code_result::pending) &&
				ec.value() != static_cast<int>(error_code_result::ok))
				buffer.pubseekpos(pos, std::ios::out);

			return ec;
		}

	private:
		static error_code normalize(flex_buffer_t& buffer, std::shared_ptr<basic_session> session_ptr,
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

			if (result.value() == static_cast<int>(error_code_result::ok))
				session_ptr->detach(proto);

			return buffer.size() != 0 ? process(buffer, uid) : result;
		}
	};
} // namespace aquarius