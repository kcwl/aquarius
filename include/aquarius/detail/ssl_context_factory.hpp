#pragma once
#ifdef AQUARIUS_ENABLE_SSL
#include <boost/asio/ssl/context.hpp>
#endif

namespace aquarius
{
	template <typename T, std::size_t SSLVersion>
	struct ssl_context_factory;
#ifdef AQUARIUS_ENABLE_SSL
	template <>
	struct ssl_context_factory<boost::asio::ssl::context&, boost::asio::ssl::context::sslv23>
	{
		static boost::asio::ssl::context& create()
		{
			static boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);

			ssl_context.set_options(boost::asio::ssl::context::default_workarounds |
									boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::single_dh_use);

			ssl_context.use_certificate_chain_file("crt/server.crt");
			ssl_context.use_private_key_file("crt/server.key", boost::asio::ssl::context::pem);
			ssl_context.use_tmp_dh_file("crt/dh512.pem");

			return ssl_context;
		}
	};
#endif
	template <typename T>
	struct ssl_context_factory<T, std::size_t(-1)>
	{
		static T& create()
		{
			static T value{};

			return value;
		}
	};
} // namespace aquarius