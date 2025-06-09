#pragma once
#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/ssl/ssl.hpp>
#include <aquarius/ssl_factory.hpp>

namespace aquarius
{
	template <bool Server>
	struct ssl_context_factory<Server, ssl::context::sslv23>
	{
		static ssl::context& create()
		{
			static ssl::context ssl_context(ssl::context::sslv23);

			if constexpr (Server)
			{
				ssl_context.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
										ssl::context::single_dh_use);

				ssl_context.use_certificate_chain_file("crt/server.crt");
				ssl_context.use_private_key_file("crt/server.key", ssl::context::pem);
				ssl_context.use_tmp_dh_file("crt/dh512.pem");
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
} // namespace aquarius
#endif