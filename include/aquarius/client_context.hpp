#pragma once
#include <aquarius/tcp/context_router.hpp>

namespace aquarius
{
	struct client_invoke
	{
		template<typename Session>
		static void invoke(std::size_t context_id, flex_buffer buffer, std::shared_ptr<Session>)
		{
			tcp::client_invokes::get_mutable_instance().invoke(context_id, buffer);
		}

		template<typename Response, typename  Func>
		static void regist(std::size_t context_id, Func&& func)
		{
			tcp::client_invokes::get_mutable_instance().template regist<Response>(context_id, std::forward<Func>(func));
		}
	};
} // namespace aquarius