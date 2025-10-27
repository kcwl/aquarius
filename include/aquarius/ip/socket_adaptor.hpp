#pragma once
#include <aquarius/detail/ssl_context.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{
	template <bool Server, typename Protocol>
	requires(is_protocol<Protocol>)
	class socket_adaptor
	{
		using socket = typename Protocol::socket;

		using resolver = typename Protocol::resolver;

	public:
		socket_adaptor(socket& s)
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
		auto accept(Func&& f) -> awaitable<error_code>
		{
			co_return co_await f();
		}

	private:
		socket& socket_;
	};

	template <bool Server, typename Protocol>
	requires(is_protocol<Protocol>)
	class ssl_socket_adaptor
	{
	public:
		using socket = typename Protocol::socket;

		using resolver = typename Protocol::resolver;

		using ssl_socket = boost::asio::ssl::stream<socket&>;

		using ssl_context = detail::ssl_context_factory<Server>;

	public:
		ssl_socket_adaptor(socket& socket)
			: socket_(socket, ssl_context::create())
		{
			ssl_context::init(socket_);
		}

	public:
		ssl_socket& get_implement()
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
				XLOG_ERROR() << "connect " << host << " failed! maybe" << ec.what();
			}

			if (ec)
				co_return ec;

			co_await socket_.async_handshake(boost::asio::ssl::stream_base::client, redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "connect " << host << " failed! maybe" << ec.what();
			}

			co_return ec;
		}

		template <typename Func>
		requires(is_awaitable_func<Func>)
		auto accept(Func&& f) -> awaitable<error_code>
		{
			if constexpr (Server)
			{
				error_code ec;

				co_await socket_.async_handshake(boost::asio::ssl::stream_base::server,
												 redirect_error(use_awaitable, ec));

				if (ec)
					co_return ec;
			}

			co_return co_await f();
		}

	private:
		ssl_socket socket_;
	};
} // namespace aquarius