#pragma once
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

		template <typename T>
		class transfer_module : public _module<transfer_module<T>, transfer_config>
		{
		public:
			transfer_module(const std::string& name)
				: _module<transfer_module<T>, transfer_config>(name)
			{}

		public:
			virtual auto run(io_context& ios) -> awaitable<void> override
			{
				transfer_ptr_ = std::make_shared<T>(ios);

				auto result = co_await transfer_ptr_->async_connect(this->configs().ip_addr, this->configs().port);

				if (!result)
				{
					XLOG_ERROR() << "transfer connect error!";
				}
			}

			auto async_sendback(flex_buffer& buffer) -> awaitable<flex_buffer>
			{
				auto req = std::make_shared<transfer_tcp_request>();

				std::copy((char*)buffer.data().data(), (char*)buffer.data().data() + buffer.data().size(), std::back_inserter(req->body().feedbuf));

				auto resp =  co_await transfer_ptr_->async_send<transfer_tcp_response>(req);

				flex_buffer resp_buffer{};
				resp_buffer.sputn(resp.body().feedbuf.data(), resp.body().feedbuf.size());

				co_return std::move(resp_buffer);
			}

		private:
			std::shared_ptr<T> transfer_ptr_;
		};
	} // namespace gateway
} // namespace aquarius