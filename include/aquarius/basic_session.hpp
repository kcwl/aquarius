#pragma once
#include <aquarius/any_io_executor.hpp>
#include <aquarius/awaitable.hpp>
#include <aquarius/concepts.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/session_object_impl.hpp>
#include <aquarius/detail/session_service.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/flex_buffer.hpp>
#include <span>

#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/detail/ssl_session_service.hpp>
#endif

namespace aquarius
{
	template <bool Server, typename Protocol>
	class basic_session : public std::enable_shared_from_this<basic_session<Server, Protocol>>
	{
	public:
#ifdef AQUARIUS_ENABLE_SSL
		using socket_type = typename detail::ssl_session_service<Server, Protocol>::ssl_socket_type;
		using impl_type = detail::session_object_impl<detail::ssl_session_service<Server, Protocol>>;
#else
		using socket_type = typename detail::session_service<Protocol>::socket_type;
		using impl_type = detail::session_object_impl<detail::session_service<Protocol>>;
#endif

		using acceptor = typename Protocol::acceptor;

	public:
		explicit basic_session(socket_type socket)
			: impl_(std::move(socket))
		{}

		virtual ~basic_session() = default;

	public:
		std::string remote_address() const
		{
			return impl_.get_service().remote_address(impl_.get_implementation());
		}

		uint32_t remote_address_u() const
		{
			return impl_.get_service().remote_address_u(impl_.get_implementation());
		}

		uint16_t remote_port() const
		{
			return impl_.get_service().remote_port(impl_.get_implementation());
		}

		auto protocol() -> awaitable<error_code>
		{
			co_await impl_.get_service().start(impl_.get_implementation());

			error_code ec;

			for (;;)
			{
				co_await Protocol::read(this->shared_from_this(), ec);

				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.what();
					}

					shutdown();

					co_return ec;
				}
			}

			std::unreachable();
		}

		template <typename BufferSequence>
		auto async_read(BufferSequence& buff) -> awaitable<error_code>
		{
			error_code ec;

			co_await impl_.get_service().async_read(impl_.get_implementation(), buff, ec);

			co_return ec;
		}

		template <typename BufferSequence>
		auto async_send(BufferSequence buffer, error_code& ec) -> awaitable<error_code>
		{
			co_await impl_.get_service().async_write_some(impl_.get_implementation(), std::move(buffer), ec);

			if (ec)
			{
				XLOG_ERROR() << "async write is failed! maybe " << ec.message();
			}

			co_return ec;
		}

		void shutdown()
		{
			impl_.get_service().shutdown();

			if (close_func_)
			{
				close_func_();
			}
		}

		void close()
		{
			impl_.get_service().close(impl_.get_implementation());

			if (close_func_)
			{
				close_func_();
			}
		}

		template <typename Func>
		void regist_close(Func&& f)
		{
			close_func_ = std::forward<Func>(f);
		}

		const auto& get_executor()
		{
			return impl_.get_executor();
		}

	private:
		impl_type impl_;

		std::function<void()> close_func_;
	};
} // namespace aquarius