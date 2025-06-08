#pragma once
#include <aquarius/concepts.hpp>
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/detail/session_object_impl.hpp>
#include <aquarius/detail/session_service.hpp>

#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/detail/ssl_session_service.hpp>
#endif

namespace aquarius
{
	template <bool Server, typename Protocol, typename Processor, typename Executor = boost::asio::any_io_executor>
	class basic_session : public std::enable_shared_from_this<basic_session<Server, Protocol, Processor, Executor>>
	{
	public:
		using executor_type = Executor;

#ifdef AQUARIUS_ENABLE_SSL
		using socket_type = typename detail::ssl_session_service<Server, Protocol, executor_type>::ssl_socket_type;
		using impl_type =
			detail::session_object_impl<detail::ssl_session_service<Server, Protocol, executor_type>, executor_type>;
#else
		using socket_type = typename detail::session_service<Protocol, executor_type>::socket_type;
		using impl_type = detail::session_object_impl<detail::session_service<Protocol, executor_type>, executor_type>;
#endif

		using process_type = Processor;

		using protocol_type = Protocol;

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

		auto async_read() -> boost::asio::awaitable<boost::system::error_code>
		{
			co_await impl_.get_service().start(impl_.get_implementation());

			boost::system::error_code ec;

			for (;;)
			{
				auto read_buffer = co_await impl_.get_service().async_read_some(impl_.get_implementation(), ec);

				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.message();
					}

					shutdown();

					co_return ec;
				}

				boost::asio::co_spawn(get_executor(), processor_.read(std::move(read_buffer)), boost::asio::detached);
			}

			std::unreachable();
		}

		auto async_send(std::size_t proto, flex_buffer fs) -> boost::asio::awaitable<void>
		{
			boost::system::error_code ec;

			auto buffers = processor_.write(proto, fs);

			for (auto& buf : buffers)
			{
				co_await async_send(buf, ec);

				if (ec)
				{
					XLOG_ERROR() << "async write is failed! maybe " << ec.message();
					break;
				}
			}
		}

		auto async_send(flex_buffer buf, error_code& ec) -> boost::asio::awaitable<error_code>
		{
			co_await impl_.get_service().async_write_some(impl_.get_implementation(), buf, ec);

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

		template<typename Func>
		void regist_close(Func&& f)
		{
			close_func_ = std::forward<Func>(f);
		}

		const executor_type& get_executor()
		{
			return impl_.get_executor();
		}

	private:
		impl_type impl_;

		process_type processor_;

		std::function<void()> close_func_;
	};
} // namespace aquarius