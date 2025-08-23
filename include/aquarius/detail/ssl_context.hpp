#pragma once
#include <boost/asio/ssl.hpp>
#include <filesystem>

namespace aquarius
{
	namespace detail
	{
		template <bool Server, auto SSLVersion = boost::asio::ssl::context::sslv23>
		struct ssl_context_factory
		{
			static boost::asio::ssl::context& create()
			{
				static boost::asio::ssl::context ssl_context(SSLVersion);

				if constexpr (Server)
				{
					ssl_context.set_options(boost::asio::ssl::context::default_workarounds |
											boost::asio::ssl::context::no_sslv2 |
											boost::asio::ssl::context::single_dh_use);
					ssl_context.set_password_callback(std::bind(&ssl_context_factory::get_passwd));
					ssl_context.use_certificate_chain_file("crt/server.crt");
					ssl_context.use_private_key_file("crt/server.key", boost::asio::ssl::context::pem);
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

		private:
			static std::string get_passwd()
			{
				return "kwcl";
			}
		};

	} // namespace detail
} // namespace aquarius