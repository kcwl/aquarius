#pragma once
#include <aquarius/module/module.hpp>

namespace aquarius
{
	namespace gateway
	{
		template <typename Session>
		struct auto_transfer_handler_register
		{
			constexpr static auto __transfer__ = "transfer";

			explicit auto_transfer_handler_register()
			{
				auto func = [&]<typename Func>(flex_buffer& buffer, Func&& f)
				{
					schedule_module("serviced_client",
									std::make_shared<transfer_task<Func>>(buffer, std::forward<Func>(f)));
				};

				router<proto_tag::tcp, Session>::get_mutable_instance().regist(__transfer__, func);
			}
		};
	} // namespace gateway
} // namespace aquarius

#define TRANSFER_HANDLER(__session)                                                                                    \
	static aquarius::gateway::auto_transfer_handler_register<__session> __auto_register_transfer;