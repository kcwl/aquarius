#pragma once
#include <aquarius/context/generator.hpp>

namespace aquarius
{
	class basic_context;
}

namespace aquarius
{
	template <typename _Context>
	struct ctx_regist
	{
		ctx_regist(std::size_t key)
		{
			context_generator::instance().regist(key, []()
										  { return std::dynamic_pointer_cast<basic_context>(std::make_shared<_Context>()); });
		}
	};

	struct invoke_context_helper
	{
		template <typename... _Args>
		static auto invoke(std::size_t key, _Args&... args)
		{
			return context_generator::instance().invoke(key, args...);
		}
	};
} // namespace aquarius