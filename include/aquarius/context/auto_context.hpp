#pragma once
#include <aquarius/context/detail/client_router.hpp>
#include <aquarius/context/detail/context_router.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Session, template<typename> typename Context>
		struct auto_context_register
		{
			explicit auto_context_register()
			{
				detail::context_router<Session>::get_mutable_instance().template regist<Context<Session>>();
			}
		};

		template <typename Session>
		struct auto_context_invoke
		{
			static void apply(auto mode, flex_buffer buff, std::size_t proto, std::shared_ptr<Session> session)
			{
				detail::context_router<Session>::get_mutable_instance().invoke(mode, buff, proto, session);
			}
		};

		struct client_invoke
		{
			template <typename Response, typename Func>
			static void regist(std::size_t context_id, Func&& func)
			{
				detail::client_router::get_mutable_instance().template regist<Response>(context_id,
																						std::forward<Func>(func));
			}

			template <typename Session>
			static void invoke(std::size_t context_id, flex_buffer buffer, std::shared_ptr<Session>)
			{
				detail::client_router::get_mutable_instance().invoke(context_id, buffer);
			}
		};
	} // namespace context
} // namespace aquarius