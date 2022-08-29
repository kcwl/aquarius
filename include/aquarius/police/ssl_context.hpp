#pragma once
#ifdef ENABLE_SSL
#include <boost/asio/ssl.hpp>
#endif
#include "../core/singleton.hpp"

namespace aquarius
{
	class ssl_context : public core::singleton<ssl_context>
	{
#ifdef ENABLE_SSL
	public:
		auto& context()
		{
			return ssl_context_;
		}

	protected:
		ssl_context()
			: ssl_context_(boost::asio::ssl::context::sslv23)
		{
			init();
		}

	private:
		void init()
		{
			boost::system::error_code ec;

			std::string certificate_chain_file;
			std::string private_key_file;

			ssl_context_.set_options(boost::asio::ssl::context::no_sslv3, ec);
			ssl_context_.use_certificate_chain_file(certificate_chain_file, ec);
			ssl_context_.use_private_key_file(private_key_file, boost::asio::ssl::context::pem, ec);
		}

	private:
		boost::asio::ssl::context ssl_context_;
#endif
	};
}