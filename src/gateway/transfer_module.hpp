#pragma once
#include <aquarius/module/module.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

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
			{

			}

		public:
			virtual auto run(io_context& ios) -> awaitable<void> override
			{
				transfer_ptr_ = std::make_shared<T>(ios);

				co_await transfer_ptr_->async_connect(this->configs().ip_addr, this->configs().port);
			}

			auto async_sendback(flex_buffer& buffer) -> awaitable<flex_buffer>
			{
				co_return co_await transfer_ptr_->async_sendback(buffer);
			}

		private:
			std::shared_ptr<T> transfer_ptr_;
		};
	} // namespace gateway
} // namespace aquarius