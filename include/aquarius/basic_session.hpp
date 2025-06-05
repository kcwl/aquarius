#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/detail/session_object_impl.hpp>
#include <aquarius/concepts.hpp>

namespace aquarius
{
	template <typename IO, typename Processor>
	class basic_session : public std::enable_shared_from_this<basic_session<IO, Processor>>
	{
		using executor_type = typename IO::executor_type;

		using socket_type = typename IO::socket_type;

		using protocol_type = typename IO::protocol_type;

		using process_type = Processor;

	public:
		explicit basic_session(socket_type socket)
			: impl_(std::move(socket))
		{}

		template <execution_context_convertible ExecutionContext>
		explicit basic_session(ExecutionContext& context)
			: impl_(std::move(socket_type(context)))
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

		auto start() -> boost::asio::awaitable<void>
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

					co_return;
				}

				auto [proto, buffer] = processor_.read(read_buffer);

				if (!buffer.empty())
				{
					tcp::invoke_context(proto, buffer, this->shared_from_this());
				}
			}
		}

		auto async_connect(std::string ip_addr, std::string port, boost::system::error_code& ec) -> boost::asio::awaitable<void>
		{
			co_await impl_.get_service().async_connect(impl_.get_implementation(), ip_addr, port, ec);

			if (ec)
			{
				co_return;
			}

			co_return co_await start();
		}

		boost::asio::awaitable<void> send_packet(std::size_t proto, flex_buffer fs)
		{
			boost::system::error_code ec;

			auto buffers = processor_.write(proto, fs);

			for (auto& buf : buffers)
			{
				co_await impl_.get_service().async_write_some(impl_.get_implementation(), buf, ec);

				if (ec)
				{
					XLOG_ERROR() << "async write is failed! maybe " << ec.message();
					break;
				}
			}
		}

		void shutdown()
		{
			return impl_.get_service().shutdown();
		}

		const executor_type& get_executor()
		{
			return impl_.get_executor();
		}

	private:
		detail::session_object_impl<IO, executor_type> impl_;

		process_type processor_;
	};
} // namespace aquarius