#pragma once
#include <boost/asio/ssl/context.hpp>

namespace aquarius
{
	inline static boost::asio::ssl::context& create_sslv23_context()
	{
		static boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);

		ssl_context.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
								boost::asio::ssl::context::single_dh_use);

		ssl_context.use_certificate_chain_file("crt/server.crt");
		ssl_context.use_private_key_file("crt/server.key", boost::asio::ssl::context::pem);
		ssl_context.use_tmp_dh_file("crt/dh512.pem");

		return ssl_context;
	}
} // namespace aquarius