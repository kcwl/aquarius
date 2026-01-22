#pragma once
#include <aquarius/ip/tcp/tcp_client.hpp>
#include <aquarius/module/module.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <proto/channel.virgo.h>

namespace aquarius
{
	namespace gateway
	{
		struct transfer_config
		{
			constexpr static auto path = "transfer"sv;

			std::string ip_addr;
			std::string port;
		};

		class transfer_module : public _module<transfer_module, transfer_config>
		{
		public:
			transfer_module(const std::string& name);

		public:
			virtual auto run(io_context& ios) -> awaitable<void> override;

			auto async_sendback(flex_buffer& buffer) -> awaitable<flex_buffer>;

		private:
			std::shared_ptr<tcp_client> transfer_ptr_;
		};
	} // namespace gateway
} // namespace aquarius