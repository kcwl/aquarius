#pragma once
#include <aquarius/tcp/context_router.hpp>
#include <aquarius/tcp/handler_router.hpp>

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
		struct auto_handler_register
		{
			explicit auto_handler_register(std::size_t proto)
			{
				handler_router<Session>::get_mutable_instance().template regist<Request, Context>(proto);
			}
		};

		template <typename Session, typename Context>
		struct auto_context_register
		{
			explicit auto_context_register()
			{
				context_router<Session>::get_mutable_instance().template regist<Context>();
			}
		};
	} // namespace tcp
} // namespace aquarius