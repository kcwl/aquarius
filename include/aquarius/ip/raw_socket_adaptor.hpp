#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <auto Tag>
	class raw_socket_adaptor
	{
		using socket = typename protocol<Tag>::socket;

		using resolver = typename protocol<Tag>::resolver;

	public:
		raw_socket_adaptor(socket& s)
			: socket_(s)
		{}

	public:
		socket& get_implement()
		{
			return socket_;
		}

		auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code>
		{
			resolver resolve(socket_.lowest_layer().get_executor());

			auto endpoints = resolve.resolve(host, port);

			error_code ec;

			co_await boost::asio::async_connect(socket_.lowest_layer(), endpoints, redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "connect " << host << " failed! " << ec.what();
			}

			co_return ec;
		}

		template <typename Func>
		requires(is_awaitable_func<Func>)
		auto accept(Func&& f) -> awaitable<void>
		{
			co_return co_await f();
		}

	private:
		socket& socket_;
	};
} // namespace aquarius