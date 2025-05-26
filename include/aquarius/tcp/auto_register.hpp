#pragma once
#include <aquarius/tcp/context_router.hpp>

namespace aquarius
{
	namespace tcp
	{
		template <typename T>
		struct hash
		{
			constexpr std::size_t operator()(const T& x) const
			{
				return std::hash<T>()(x);
			}
		};

		template <typename Session, typename Request, typename Context>
		struct auto_register
		{
			explicit auto_register(std::size_t proto)
			{
				context_router<Session>::get_mutable_instance().template regist<Request, Context>(proto);
			}
		};

		template <typename Session>
		inline void invoke_context(std::size_t proto, flex_buffer& buffer, std::shared_ptr<Session> session)
		{
			boost::asio::post(session->get_executor(), [proto, buf = std::move(buffer), session]() mutable
							  { context_router<Session>::get_mutable_instance().invoke(proto, buf, session); });
		}
	} // namespace tcp
} // namespace aquarius