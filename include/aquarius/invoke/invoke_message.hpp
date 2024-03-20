#pragma once
#include <aquarius/invoke/router_message.hpp>

namespace aquarius
{
	template <typename _Message>
	struct msg_regist
	{
		msg_regist(std::size_t key)
		{
			router_message::instance().regist(
				key, []() { return std::dynamic_pointer_cast<impl::basic_message>(std::make_shared<_Message>()); });
		}
	};

	struct invoke_message_helper
	{
		template <typename... _Args>
		static auto invoke(std::size_t key, _Args&... args)
		{
			return router_message::instance().invoke(key, args...);
		}
	};
} // namespace aquarius