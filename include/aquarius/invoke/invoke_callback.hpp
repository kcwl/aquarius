#pragma once
#include <aquarius/invoke/router_session.hpp>


namespace aquarius
{
	struct invoke_callback_helper
	{
		template<typename _Func>
		static void regist(std::size_t id, _Func&& f)
		{
			router_session::instance().regist(id, std::forward<_Func>(f));
		}

		static bool apply(std::size_t id, std::shared_ptr<impl::basic_message> message)
		{
			return router_session::instance().apply(id, message);
		}
	};
}