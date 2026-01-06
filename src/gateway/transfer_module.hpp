#pragma once
#include <aquarius/module/module.hpp>

namespace aquarius
{
	namespace ip
	{
		struct transfer_config
		{
			constexpr static auto name = "transfer"sv;

			std::string ip_addr;
			int16_t port;
		};

		template <typename T>
		class transfer_module : public _module<T, transfer_config>
		{
		public:
			transfer_module() = default;

		public:
			virtual bool init() override
			{
				transfer_ptr_ = std::make_shared<T>(config().ip_addr, config().port);

				return transfer_ptr_ != nullptr;
			}

			virtual auto run() -> awaitable<void> override
			{
				co_await transfer_ptr_->connect();
			}

			virtual std::shared_ptr<T> core()
			{
				return transfer_ptr_;
			}

		private:
			std::shared_ptr<T> transfer_ptr_;
		};
	} // namespace ip
} // namespace aquarius