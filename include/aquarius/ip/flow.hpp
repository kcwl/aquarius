#pragma once
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler_router.hpp>
#include <memory>

namespace aquarius
{
	namespace ip
	{
		template <bool Server>
		struct tcp_stream_flow
		{
			constexpr static bool is_server = Server;

			template <typename Session, typename BuffSequence, typename Header>
			static void recv(std::shared_ptr<Session> session_ptr, BuffSequence buffer, Header h)
			{
				if (!session_ptr)
					return;

				co_spawn(
					session_ptr->get_executor(),
					[buffer = std::move(buffer), session_ptr, h = std::move(h)]() mutable -> awaitable<void>
					{
						context::stream<basic_tcp<tcp_stream_flow>>()(h.rpc_id(), session_ptr, std::move(buffer), h);
						co_return;
					},
					detached);
			}
		};

		//template <>
		//struct tcp_stream_flow<false>
		//{
		//	constexpr static bool is_server = false;

		//	template <typename Session, typename BuffSequence, typename Header>
		//	static void recv(std::shared_ptr<Session>, BuffSequence buffer, Header h)
		//	{
		//		context::detail::client_router::get_mutable_instance().invoke(h.rpc_id(), std::move(buffer));
		//	}
		//};


		template <bool Server>
		struct tcp_transfer_flow;

		template <>
		struct tcp_transfer_flow<true>
		{
			constexpr static bool is_server = false;

			template <typename Session, typename BuffSequence, typename Header>
			static void recv(std::shared_ptr<Session> session_ptr, BuffSequence buffer, Header h)
			{
				context::transfer<basic_tcp<tcp_transfer_flow>>()(session_ptr, std::move(buffer), h);
			}
		};
	} // namespace ip
} // namespace aquarius