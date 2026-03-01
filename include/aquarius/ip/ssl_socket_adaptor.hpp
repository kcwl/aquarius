#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/ssl_context.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	template <bool Server, typename Socket, typename Resolver>
	class ssl_socket_adaptor
	{
		template <auto SSLVersion = ssl::context::sslv23>
		struct ssl_context_factory
		{
			static ssl::context& create()
			{
				static ssl::context ssl_context(SSLVersion);

				if constexpr (Server)
				{
					ssl_context.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
											ssl::context::single_dh_use);
					// ssl_context.set_password_callback(std::bind(&ssl_context_factory::get_passwd));
					ssl_context.use_certificate_chain_file("crt/server.crt");
					ssl_context.use_private_key_file("crt/server.key", ssl::context::pem);
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

			template <typename SSLSocket>
			requires(is_ssl_socket<SSLSocket>)
			static void init(SSLSocket& ssl_socket)
			{
				if constexpr (!Server)
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
			}
		};

	public:
		using socket_t = Socket;

		using resolver_t = Resolver;

		using ssl_socket = ssl::stream<socket_t&>;

		using ssl_context = ssl_context_factory<Server>;

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

		template <typename Dura>
		auto async_connect(const std::string& host, const std::string& port, Dura timeout) -> awaitable<error_code>
		{
			resolver resolve(socket_.lowest_layer().get_executor());

			auto endpoints = resolve.resolve(host, port);

			error_code ec;

			co_await async_connect(socket_.lowest_layer(), endpoints,
								   cancel_after(timeout, redirect_error(use_awaitable, ec)));

			if (!ec)
			{
				co_await socket_.async_handshake(ssl::stream_base::client,
												 cancel_after(timeout, redirect_error(use_awaitable, ec)));
			}

			co_return ec;
		}

		template <typename Dura>
		auto accept(Dura timeout) -> awaitable<error_code>
		{
			error_code ec;

			co_await socket_.async_handshake(ssl::stream_base::server,
											 cancel_after(timeout, redirect_error(use_awaitable, ec)));

			co_return ec;
		}

	private:
		ssl_socket socket_;
	};
} // namespace aquarius