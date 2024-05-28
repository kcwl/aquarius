#pragma once
#include <aquarius/core/asio.hpp>
#include <aquarius/core/logger.hpp>

namespace aquarius
{
	template<typename _ServerType>
	class ssl
	{
		constexpr static bool is_server = _ServerType::value;

	public:
		explicit ssl(asio::socket_t& socket, asio::ssl_context_t& ctx)
			: socket_(socket)
			, ssl_context_(ctx)
			, ssl_socket_(socket_, ssl_context_)
		{
			if constexpr (!is_server)
			{
				ssl_socket_.set_verify_mode(asio::ssl::verify_peer);
			}
		}

	public:
		template<typename _Func>
		void start(_Func&& f)
		{
			ssl_socket_.async_handshake(static_cast<asio::ssl::stream_base::handshake_type>(is_server),
										[this, func = std::move(f)](const asio::error_code& ec)
										{
											if (ec)
											{
												XLOG_ERROR() << "handshake error, maybe " << ec.message();

												return;
											}

											func();
										});
		}

		template<typename _Buffer, typename _Func>
		void async_read_some(const _Buffer& buffer, _Func&& f)
		{
			return ssl_socket_.async_read_some(buffer, std::forward<_Func>(f));
		}

		template<typename _Buffer, typename _Func>
		void async_write_some(const _Buffer& buffer, _Func&& f)
		{
			return ssl_socket_.async_write_some(buffer, std::forward<_Func>(f));
		}

		void shut_down()
		{
			if (!socket_.is_open())
				return;

			asio::error_code ec{};
			socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

			socket_.close(ec);

			ssl_socket_.shutdown(ec);
		}

	private:
		asio::socket_t& socket_;

		asio::ssl_context_t& ssl_context_;

		asio::ssl_socket_t ssl_socket_;
	};
} // namespace aquarius