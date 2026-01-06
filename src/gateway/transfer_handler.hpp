#pragma once
#include "transfer_schedule.hpp"
#include <aquarius/module/module.hpp>

namespace aquarius
{
	namespace gateway
	{
		template <typename T, auto Tag, typename Session>
		struct auto_transfer_handler_register
		{
			explicit auto_transfer_handler_register()
			{
				auto func = [&](std::shared_ptr<Session> session, flex_buffer& buffer)
				{
					co_spawn(
						session->get_executor(),
						[&] -> awaitable<void>
						{
							auto resp_buffer = co_await ip::mpu_transfer<T>(buffer);

							co_await session->async_send(resp_buffer);
						},
						detached);

					return true;
				};

				router<Tag, Session>::get_mutable_instance().regist(__transfer__, func);
			}
		};

		template <typename T, typename Session>
		struct auto_transfer_handler_register<T, proto_tag::http, Session>
		{
			explicit auto_transfer_handler_register()
			{
				auto func = [&](std::shared_ptr<Session> session, virgo::http_fields, flex_buffer& buffer)
				{
					co_spawn(
						session->get_executor(),
						[&] -> awaitable<void>
						{
							auto resp_buffer = co_await ip::mpu_transfer<T>(buffer);

							co_await session->async_send(resp_buffer);
						},
						detached);

					return true;
				};

				router<proto_tag::http, Session>::get_mutable_instance().regist(__transfer__, func);
			}
		};
	} // namespace gateway
} // namespace aquarius

#define TRANSFER_HANDLER(__session, __op)                                                                              \
	static aquarius::gateway::auto_transfer_handler_register<__op, __session::tag, __session> __auto_register_transfer;