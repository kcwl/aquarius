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
			router_context::instance().regist(key, []()
										  { return std::dynamic_pointer_cast<basic_context>(std::make_shared<_Context>()); });
		}
	};

	struct invoke_context_helper
	{
		template <typename... _Args>
		static auto invoke(std::size_t key, _Args&... args)
		{
			return router_context::instance().invoke(key, args...);
		}
	};
} // namespace aquarius