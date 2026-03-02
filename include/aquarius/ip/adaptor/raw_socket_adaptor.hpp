#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <typename Socket>
	class raw_socket_adaptor
	{
		using socket_t = Socket;

	public:
		raw_socket_adaptor(socket_t& socket)
			: socket_(socket)
		{}

	public:
		socket_t& get_implement()
		{
			return socket_;
		}

		template <typename Endpoint, typename Dura>
		auto async_connect(const Endpoint& endpoint, Dura timeout) -> awaitable<error_code>
		{
			error_code ec;

			co_await boost::asio::async_connect(socket_, endpoint,
												cancel_after(timeout, redirect_error(use_awaitable, ec)));

			co_return ec;
		}

		template <typename Dura>
		auto accept(Dura) -> awaitable<error_code>
		{
			co_return error_code{};
		}

	private:
		socket_t& socket_;
	};
} // namespace aquarius