#pragma once
#include <aquarius/message/router.hpp>
#include <cstdint>

namespace aquarius
{
	class context;

	template <typename _Context>
	struct ctx_regist
	{
		ctx_regist(const std::size_t& key)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			ctx_router::instance().regist(_key, []()
										  { return std::dynamic_pointer_cast<context>(std::make_shared<_Context>()); });
		}
	};

	struct context_invoke_helper
	{
		template <typename... _Args>
		static auto invoke(uint32_t key, _Args&... args)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			return ctx_router::instance().invoke(_key, args...);
		}
	};

	template <typename _Message>
	struct msg_regist
	{
		msg_regist(std::size_t key)
		{
			msg_router::instance().regist(
				key, []() { return std::dynamic_pointer_cast<xmessage>(std::make_shared<_Message>()); });
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

	inline std::vector<std::size_t> count_message_ids()
	{
		std::vector<std::size_t> result{};

		msg_router::instance().count_ids(result);
	}

} // namespace aquarius