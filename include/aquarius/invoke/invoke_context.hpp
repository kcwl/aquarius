#pragma once
#include <aquarius/invoke/router_context.hpp>

namespace aquarius
{
	class basic_context;
}

namespace aquarius
{
	template <typename _Context>
	struct ctx_regist
	{
		ctx_regist(const std::size_t& key)
		{
			ctx_router::instance().regist(key, []()
										  { return std::dynamic_pointer_cast<basic_context>(std::make_shared<_Context>()); });
		}
	};

	struct context_invoke_helper
	{
		template <typename... _Args>
		static auto invoke(uint32_t key, _Args&... args)
		{
			return ctx_router::instance().invoke(key, args...);
		}
	};
} // namespace aquarius