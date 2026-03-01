#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <bool, typename Socket, typename Resolver>
	class raw_socket_adaptor
	{
		using socket_t = Socket;

		using resolver_t = Resolver;

	public:
		raw_socket_adaptor(socket_t& socket)
			: socket_(socket)
		{}

	public:
		socket_t& get_implement()
		{
			return socket_;
		}

		template <typename Dura>
		auto async_connect(const std::string& host, const std::string& port, Dura timeout) -> awaitable<error_code>
		{
			resolver_t resolve(socket_.lowest_layer().get_executor());

			auto endpoints = resolve.resolve(host, port);

			error_code ec;

			co_await boost::asio::async_connect(socket_.lowest_layer(), endpoints,
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