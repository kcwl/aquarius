#pragma once
#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/awaitable.hpp>
#include <aquarius/detail/ssl_context.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/ssl.hpp>

namespace aquarius
{
	namespace detail
	{
		template <bool Server, typename Protocol, auto SSLVersion = boost::asio::ssl::context::sslv23>
		class basic_ssl_session_service : public boost::asio::detail::execution_context_service_base<
											  basic_ssl_session_service<Server, Protocol, SSLVersion>>
		{
		public:
			using execution_base_type = boost::asio::detail::execution_context_service_base<
				basic_ssl_session_service<Server, Protocol, SSLVersion>>;

			using socket = typename Protocol::socket;

			using ssl_socket_type = boost::asio::ssl::stream<socket>;

			struct implementation_type
			{
				ssl_socket_type* ssl_socket;
			};

		public:
			basic_ssl_session_service(boost::asio::execution_context& context)
				: execution_base_type(context)
			{

			}

			virtual ~basic_ssl_session_service() = default;

		public:
			void construct(implementation_type& /*impl*/)
			{
				return;
			}

			void move_copy(implementation_type& impl, socket socket)
			{
				impl.ssl_socket =
					new ssl_socket_type(std::move(socket), ssl_context_factory<Server, SSLVersion>::create());

				impl.ssl_socket->set_verify_mode(boost::asio::ssl::verify_peer);
				
				impl.ssl_socket->set_verify_callback([] (bool verify, boost::asio::ssl::verify_context& ctx) 
													{
														char subject_name[256];
														X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
														X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
														std::cout << "Verifying " << subject_name << "\n";

														return verify;
													});
			}

			void destroy(implementation_type& impl)
			{
				error_code ec{};

				impl.ssl_socket->shutdown(ec);

				delete impl.ssl_socket;

				impl.ssl_socket = nullptr;
			}

			virtual void shutdown() override
			{
				return;
			}

			std::string remote_address(const implementation_type& impl) const
			{
				return impl.ssl_socket->lowest_layer().remote_endpoint().address().to_string();
			}

			uint32_t remote_address_u(const implementation_type& impl) const
			{
				return impl.ssl_socket->lowest_layer().remote_endpoint().address().to_v4().to_uint();
			}

			uint16_t remote_port(const implementation_type& impl) const
			{
				return impl.ssl_socket->lowest_layer().remote_endpoint().port();
			}

			bool keep_alive(implementation_type& impl, bool value)
			{
				error_code ec;

				impl.ssl_socket->lowest_layer().set_option(boost::asio::socket_base::keep_alive(value), ec);

				return !ec;
			}

			bool set_nodelay(implementation_type& impl, bool enable)
			{
				error_code ec;
				impl.ssl_socket->lowest_layer().set_option(typename Protocol::no_delay(enable), ec);

				return !ec;
			}

			template <typename BufferSequence>
			auto async_read_some(implementation_type& impl, BufferSequence& buffer, error_code& ec) -> awaitable<void>
			{
				co_await impl.ssl_socket->async_read_some(boost::asio::buffer(buffer),
														  redirect_error(use_awaitable, ec));
			}

			template <typename BufferSequence>
			auto async_read(implementation_type& impl, BufferSequence& buff, error_code& ec) -> awaitable<void>
			{
				co_await boost::asio::async_read(*impl.ssl_socket, buffer(buff), redirect_error(use_awaitable, ec));
			}

			template <typename BufferSequence>
			auto async_write_some(implementation_type& impl, BufferSequence buff, error_code& ec)
				-> awaitable<std::size_t>
			{
				co_return co_await impl.ssl_socket->async_write_some((buffer)(buff), redirect_error(use_awaitable, ec));
			}

			auto start(implementation_type& impl) -> awaitable<void>
			{
				error_code ec;

				if constexpr (Server)
				{
					co_await impl.ssl_socket->async_handshake(boost::asio::ssl::stream_base::server,
															  redirect_error(use_awaitable, ec));
				}
				else
				{
					co_await impl.ssl_socket->async_handshake(boost::asio::ssl::stream_base::client,
															  redirect_error(use_awaitable, ec));
				}
			}
		};

	} // namespace detail
} // namespace aquarius
#endif