#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/adaptor/detail/ssl_context_base.hpp>

namespace aquarius
{
	template <auto SSLVersion>
	class basic_ssl_server_context : public ssl_context_base
	{
	public:
		virtual ssl::context create() override
		{
			ssl::context ssl_context(SSLVersion);

			ssl_context.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
									ssl::context::single_dh_use);
			// ssl_context.set_password_callback(std::bind(&ssl_context_factory::get_passwd));
			ssl_context.use_certificate_chain_file("crt/server.crt");
			ssl_context.use_private_key_file("crt/server.key", ssl::context::pem);
			ssl_context.use_tmp_dh_file("crt/dh2048.pem");

			return ssl_context;
		}
	};

	template <typename Socket, auto SSLVersion = ssl::context::sslv23>
	class ssl_server_socket_adaptor : public basic_ssl_server_context<SSLVersion>
	{
	public:
		using socket = Socket;

		using ssl_socket = ssl::stream<socket&>;

	public:
		ssl_server_socket_adaptor(socket& socket)
			: socket_(socket, this->create())
		{}

	public:
		ssl_socket& get_implement()
		{
			return socket_;
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