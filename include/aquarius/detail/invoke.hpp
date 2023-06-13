#pragma once
#include <aquarius/router.hpp>
#include <cstdint>

namespace aquarius
{
	namespace detail
	{
		template <typename _Context>
		struct ctx_regist
		{
			ctx_regist(const std::size_t& key)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				ctx_router::instance().regist(
					_key, []() { return std::dynamic_pointer_cast<context>(std::make_shared<_Context>()); });
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
			msg_regist(const std::size_t& key)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				msg_router::instance().regist(
					_key, []() { return std::dynamic_pointer_cast<xmessage>(std::make_shared<_Message>()); });
			}
		};

		struct message_invoke_helpter
		{
			template <typename... _Args>
			static auto invoke(uint32_t key, _Args&... args)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				return msg_router::instance().invoke(_key, args...);
			}
		};
	} // namespace detail
} // namespace aquarius