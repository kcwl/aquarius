#pragma once
#include <aquarius/basic_session.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <aquarius/awaitable.hpp>
#include <aquarius/context/auto_context.hpp>

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
			using base_type = boost::asio::ip::tcp;

			using socket_type = typename base_type::socket;

			struct local_header
			{
				uint32_t context_id;
				uint8_t mode;
			};

		public:
			using client_session = basic_session<false, tcp>;

			using server_session = basic_session<true, tcp>;

		public:
			std::size_t get_header_length() const
			{
				return sizeof(uint64_t);
			}

			std::size_t get_body_length(flex_buffer buff) const
			{
				uint64_t length{};

				buff.load((uint8_t*)&length, sizeof(length));

				return length;
			}

			template<typename Session>
			auto process(flex_buffer buff, std::shared_ptr<Session> session) -> awaitable<void>
			{
				local_header header{};
				buff.load((uint8_t*)&header, sizeof(local_header));

				context::auto_context_invoke<Session>::apply(header.mode, buff, header.context_id, session);

				co_return;
			}
		};

		using boost::asio::ip::port_type;
	} // namespace ip
} // namespace aquarius