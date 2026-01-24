#pragma once
#include "default_selector.hpp"
#include "transfer_schedule.hpp"
#include <aquarius/module/module.hpp>

namespace aquarius
{
	namespace gateway
	{
		template <typename T, typename Session>
		struct auto_transfer_handler_register
		{
			explicit auto_transfer_handler_register()
			{
				auto func = [&](std::shared_ptr<Session> session, std::shared_ptr<header_field_base> hf, flex_buffer& buffer)
				{
					co_spawn(
						session->get_executor(),
						[&] -> awaitable<void>
						{
							auto resp_buffer = co_await ip::mpu_transfer<T>(buffer, hf);

							co_await session->async_send(resp_buffer);
						},
						detached);

					return true;
				};

				router<Session>::get_mutable_instance().regist(__transfer__, func);
			}
		};
	} // namespace gateway
} // namespace aquarius

#define TRANSFER_HANDLER(__session, __op)                                                                              \
	static aquarius::gateway::auto_transfer_handler_register<__op, __session> __auto_register_transfer;