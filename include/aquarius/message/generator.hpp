#pragma once
#include <aquarius/core/generator.hpp>

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class message_generator : public single_generator<message_generator, std::shared_ptr<basic_message>>
	{};

	template <typename _Message>
	struct msg_regist
	{
		msg_regist(std::size_t key)
		{
			message_generator::instance().regist(
				key, []() { return std::dynamic_pointer_cast<basic_message>(std::make_shared<_Message>()); });
		}
	};

	struct invoke_message_helper
	{
		template <typename... _Args>
		static auto invoke(std::size_t key, _Args&... args)
		{
			return message_generator::instance().invoke(key, args...);
		}
	};
} // namespace aquarius