#pragma once
#include <aquarius/context/detail/context_router.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Session, template <typename> typename Context>
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
			static void apply(auto mode, std::vector<char> buff, std::size_t proto, std::shared_ptr<Session> session)
			{
				detail::context_router<Session>::get_mutable_instance().invoke(mode, std::move(buff), proto, session);
			}
		};
	} // namespace context
} // namespace aquarius