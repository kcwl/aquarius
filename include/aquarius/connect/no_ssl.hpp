#pragma once
#include <aquarius/core/asio.hpp>

namespace aquarius
{
	template <typename _ServerType>
	class no_ssl
	{
	public:
		template <typename... _Args>
		no_ssl(_Args&&... args)
			: no_ssl(std::forward<_Args>(args)...)
		{}

		template <typename... _Args>
		no_ssl(asio::socket_t& socket, _Args&&...)
			: socket_(socket)
		{}

	public:
		template <typename _Func>
		void start(_Func&& f)
		{
			std::forward<_Func>(f)();
		}

		template <typename _Buffer, typename _Func>
		void async_read_some(const _Buffer& buffer, _Func&& f)
		{
			return socket_.async_read_some(buffer, std::forward<_Func>(f));
		}

		template <typename _Buffer, typename _Func>
		void async_write_some(const _Buffer& buffer, _Func&& f)
		{
			return socket_.async_write_some(buffer, std::forward<_Func>(f));
		}

		void shut_down()
		{
			if (!socket_.is_open())
				return;

			asio::error_code ec{};
			socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

			socket_.close(ec);
		}

	private:
		asio::socket_t& socket_;
	};
} // namespace aquarius
