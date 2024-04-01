#pragma once
#include <aquarius/client/async.hpp>
#include <aquarius/core/apply.hpp>

namespace aquarius
{
	class basic_message;
}

namespace aquarius
{
	struct invoke_callback_helper
	{
		template <typename _Func>
		static void regist(std::size_t id, _Func&& f)
		{
			async_generator::instance().regist(
				id, std::bind(&invoker<_Func, basic_message>::apply, std::move(f), std::placeholders::_1));
		}

		static bool apply(std::size_t id, std::shared_ptr<basic_message> message)
		{
			return async_generator::instance().invoke(id, message);
		}
	};
} // namespace aquarius