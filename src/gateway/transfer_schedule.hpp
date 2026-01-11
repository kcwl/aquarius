#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/module/schedule.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	namespace ip
	{
		constexpr static auto __transfer__ = "transfer"sv;
		constexpr static auto __transfer_module__ = "transfer_module"sv;

		template <typename T>
		inline auto mpu_transfer(flex_buffer& buffer) -> awaitable<flex_buffer>
		{
			co_return co_await mpc::call<flex_buffer, T>(__transfer_module__,
														 [&](T* ptr) -> awaitable<flex_buffer>
														 {
															 buffer.pubseekpos(0, std::ios::out);

															 auto result = co_await ptr->async_sendback(buffer);

															 co_return std::move(result);
														 });
		}

		template <typename T>
		struct transfer_router
		{
			template <typename Session>
			static auto call(std::shared_ptr<Session> session_ptr, flex_buffer& buffer) -> awaitable<void>
			{
				co_await session_ptr->async_send(co_await mpu_transfer<T>(buffer));
			}
		};
	} // namespace ip
} // namespace aquarius