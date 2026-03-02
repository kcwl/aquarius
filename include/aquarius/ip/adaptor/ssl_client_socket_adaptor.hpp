#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/adaptor/detail/ssl_context_base.hpp>
#include <aquarius/logger.hpp>
#include <filesystem>

namespace aquarius
{

	template <auto SSLVersion>
	class basic_ssl_client_context : public ssl_context_base
	{
	public:
		virtual ssl::context create() override
		{
			ssl::context ssl_context(SSLVersion);

			auto path = std::filesystem::current_path();

			path.append("crt");
			path.append("server.crt");

			ssl_context.load_verify_file(path.string());
		}

		template <typename SSLSocket>
		requires(is_ssl_socket<SSLSocket>)
		static void init(SSLSocket& ssl_socket)
		{
			ssl_socket.set_verify_mode(ssl::verify_peer);

			ssl_socket.set_verify_callback(
				[](bool verify, ssl::verify_context& ctx)
				{
					char subject_name[256];
					X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
					X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
					XLOG_INFO() << "Verifying " << subject_name << "\n";

					return verify;
				});
		}
	};

	template <typename Socket, auto SSLVersion = ssl::context::sslv23>
	class ssl_client_socket_adaptor : public basic_ssl_client_context<SSLVersion>
	{
	public:
		using socket = Socket;

		using ssl_socket = ssl::stream<socket&>;

	public:
		ssl_client_socket_adaptor(socket& socket)
			: socket_(socket, this->create())
		{
			this->init();
		}

	public:
		ssl_socket& get_implement()
		{
			return socket_;
		}

		template <typename Endpoint, typename Dura>
		auto async_connect(const Endpoint& endpoint, Dura timeout) -> awaitable<error_code>
		{
			error_code ec;

			co_await async_connect(socket_.lowest_layer(), endpoint,
								   cancel_after(timeout, redirect_error(use_awaitable, ec)));

			if (!ec)
			{
				co_await socket_.async_handshake(ssl::stream_base::client,
												 cancel_after(timeout, redirect_error(use_awaitable, ec)));
			}

			co_return ec;
		}

	private:
		ssl_socket socket_;
	};
} // namespace aquarius