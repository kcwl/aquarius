#pragma once
#include <aquarius/basic_processor.hpp>
#include <aquarius/basic_session.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	template <typename T>
	struct basic_processor;
}

namespace aquarius
{
	namespace ip
	{
		class tcp : public boost::asio::ip::tcp
		{
		public:
			tcp() = default;

		public:
			using processor = basic_processor<tcp>;

			using client_session = basic_session<false, tcp>;

			using server_session = basic_session<true, tcp>;
		};

		using boost::asio::ip::port_type;
	} // namespace ip
} // namespace aquarius