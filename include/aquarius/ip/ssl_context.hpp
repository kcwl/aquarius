#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/logger.hpp>
#include <boost/asio/ssl.hpp>
#include <filesystem>

namespace aquarius
{
	template <bool Server, auto SSLVersion = ssl::context::sslv23>
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
} // namespace aquarius