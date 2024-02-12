#pragma once
#include <aquarius/invoke/router_message.hpp>

namespace aquarius
{
	class basic_message;
}

namespace aquarius
{
	template <typename _Message>
	struct msg_regist
	{
		msg_regist(std::size_t key)
		{
			msg_router::instance().regist(
				key, []() { return std::dynamic_pointer_cast<basic_message>(std::make_shared<_Message>()); });
		}
	};

	struct message_invoke_helper
	{
		template <typename... _Args>
		static auto invoke(uint32_t key, _Args&... args)
		{
			return msg_router::instance().invoke(key, args...);
		}
	};
} // namespace aquarius