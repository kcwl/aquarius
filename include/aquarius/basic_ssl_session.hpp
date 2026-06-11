#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/resource/ssl_config.hpp>
#include <map>

namespace aquarius
{
	template <typename Protocol, auto SSLVersion, bool Server = true>
	class basic_ssl_session
	{
	public:
		using socket_type = typename Protocol::socket_type;

		using resolver_type = typename Protocol::resolver_type;

		using duration = std::chrono::system_clock::duration;

	private:
		using ssl_socket_type = asio::ssl::stream<socket_type>;

		using ssl_context_type = asio::ssl::context;

	public:
		explicit basic_ssl_session(socket_type _socket, duration timeout)
			: socket_(std::move(_socket), create_ssl_context())
			, timeout_(timeout)
			, uuid_(detail::uuid_generator()())
		{
			init();
		}

		virtual ~basic_ssl_session() = default;

	public:
		auto get_executor()
		{
			return socket_.get_executor();
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

		duration timeout() const
		{
			return timeout_;
		}

		auto async_connect(const std::string& host, const std::string& port) -> asio::awaitable<error_code>
		{
			resolver_type resolve(this->get_executor());

			error_code ec{};

			co_await asio::async_connect(socket_.lowest_layer(), resolve.resolve(host, port),
										 asio::cancel_after(timeout_, asio::redirect_error(asio::use_awaitable, ec)));

			co_return ec;
		}

		auto async_handshake() -> asio::awaitable<error_code>
		{
			error_code ec;

			if constexpr (Server)
			{
				co_await socket_.async_handshake(
					asio::ssl::stream_base::server,
					asio::cancel_after(timeout_, asio::redirect_error(asio::use_awaitable, ec)));
			}
			else
			{
				co_await socket_.async_handshake(
					asio::ssl::stream_base::client,
					asio::cancel_after(timeout_, asio::redirect_error(asio::use_awaitable, ec)));
			}

			co_return ec;
		}

		std::string remote_address() const
		{
			return socket_.lowest_layer().remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u() const
		{
			return socket_.lowest_layer().remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port() const
		{
			return socket_.lowest_layer().remote_endpoint().port();
		}

		auto async_read(flex_buffer& buffer, std::size_t length) -> asio::awaitable<error_code>
		{
			error_code ec;

			auto mutable_buffer = buffer.prepare(length);

			co_await asio::async_read(socket_, mutable_buffer, asio::redirect_error(asio::use_awaitable, ec));

			co_return ec;
		}

		auto async_read_until(flex_buffer& buffer, std::string_view delm, std::size_t& end_pos)
			-> asio::awaitable<error_code>
		{
			error_code ec;

			end_pos =
				co_await asio::async_read_until(socket_, buffer, delm, asio::redirect_error(asio::use_awaitable, ec));

			co_return ec;
		}

		template <typename ConstBufferSequence>
		auto async_send(ConstBufferSequence&& buffers) -> asio::awaitable<error_code>
		{
			error_code ec{};

			co_await asio::async_write(socket_, buffers, asio::redirect_error(asio::use_awaitable, ec));

			co_return ec;
		}

		bool shutdown()
		{
			error_code ec;

			socket_.shutdown(ec);

			if (ec)
			{
				XLOG_ERROR() << "[shutdown] error: " << ec.what();
			}

			return !ec;
		}

		error_code keep_alive(bool enable)
		{
			error_code ec;

			socket_.lowest_layer().set_option(typename Protocol::keep_alive(enable), ec);

			return ec;
		}

		error_code set_nodelay(bool enable)
		{
			error_code ec;

			socket_.lowest_layer().set_option(typename Protocol::no_delay(enable), ec);

			return ec;
		}

	private:
		void init()
		{
			if constexpr (!Server)
			{
				socket_.set_verify_mode(asio::ssl::verify_peer);

				socket_.set_verify_callback(
					[](bool verify, asio::ssl::verify_context& ctx)
					{
						char subject_name[256];
						X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
						X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
						XLOG_INFO() << "Verifying " << subject_name << "\n";

						return verify;
					});
			}
		}

		ssl_context_type& create_ssl_context()
		{
			static ssl_context_type context(SSLVersion);

			if constexpr (Server)
			{
				static std::once_flag once{};

				std::call_once(once,
							   [&]()
							   {
								   ssl_config& cfg = create_ssl();

								   context.set_options(asio::ssl::context::default_workarounds |
													   asio::ssl::context::no_sslv2 |
													   asio::ssl::context::single_dh_use);

								   context.use_certificate_chain_file(cfg.crt);
								   context.use_private_key_file(cfg.key, asio::ssl::context::pem);
								   context.use_tmp_dh_file(cfg.pem);
							   });
			}
			else
			{
				static std::once_flag once{};

				std::call_once(once,
							   [&]()
							   {
								   ssl_config& cfg = create_ssl();

								   context.load_verify_file(cfg.crt);
							   });
			}

			return context;
		}

	private:
		ssl_socket_type socket_;

		std::size_t uuid_;

		duration timeout_;
	};
} // namespace aquarius