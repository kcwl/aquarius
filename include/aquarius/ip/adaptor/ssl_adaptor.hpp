#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <filesystem>

namespace aquarius
{
	template <auto SSLVersion, bool Server>
	class basic_ssl_context
	{
	public:
		asio::ssl::context& create()
		{
			static asio::ssl::context ssl_context(SSLVersion);

			if constexpr (Server)
			{
				ssl_context.set_options(asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 |
										asio::ssl::context::single_dh_use);

				ssl_context.use_certificate_chain_file("crt/server.crt");
				ssl_context.use_private_key_file("crt/server.key", asio::ssl::context::pem);
				ssl_context.use_tmp_dh_file("crt/dh2048.pem");
			}
			else
			{
				auto path = std::filesystem::current_path();

				path.append("crt");
				path.append("server.crt");

				ssl_context.load_verify_file(path.string());
			}

			return ssl_context;
		}
	};

	template <bool Server, typename T, auto SSLVersion = asio::ssl::context::tlsv13>
	class ssl_adaptor : public basic_ssl_context<SSLVersion, Server>
	{
	public:
		using socket = T;

		using ssl_socket = asio::ssl::stream<socket&>;

		using base = basic_ssl_context<SSLVersion, Server>;

	public:
		ssl_adaptor(socket& socket)
			: socket_(socket, this->create())
		{
			init();
		}

	public:
		ssl_socket& get_implement()
		{
			return socket_;
		}

		template <typename Endpoint, typename Dura>
		auto async_connect(const Endpoint& endpoint, Dura timeout) -> asio::awaitable<error_code>
		{
			error_code ec;

			co_await asio::async_connect(socket_.lowest_layer(), endpoint,
										 asio::cancel_after(timeout, asio::redirect_error(asio::use_awaitable, ec)));

			if (!ec)
			{
				co_await socket_.async_handshake(
					asio::ssl::stream_base::client,
					asio::cancel_after(timeout, asio::redirect_error(asio::use_awaitable, ec)));
			}

			co_return ec;
		}

		template <typename Dura>
		auto async_handshake(Dura timeout) -> asio::awaitable<error_code>
		{
			error_code ec;

			co_await socket_.async_handshake(
				asio::ssl::stream_base::server,
				asio::cancel_after(timeout, asio::redirect_error(asio::use_awaitable, ec)));

			co_return ec;
		}

	private:
		void init()
		{
			if constexpr (!Server)
			{
				socket_.set_verify_mode(asio::ssl::verify_peer);

				socket_.set_verify_callback(
					[](bool verify, asio::ssl::verify_context& ctx)
					{
						char subject_name[256];
						X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
						X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
						XLOG_INFO() << "Verifying " << subject_name << "\n";

						return verify;
					});
			}
		}

	private:
		ssl_socket socket_;
	};

	template <typename T>
	using ssl_client_adaptor = ssl_adaptor<false, T>;

	template <typename T>
	using ssl_server_adaptor = ssl_adaptor<true, T>;
} // namespace aquarius