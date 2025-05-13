#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/ssl/ssl_traits.hpp>
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	template <typename Protocol, typename Executor, typename SSLService>
	class basic_async_io_service : public boost::asio::detail::execution_context_service_base<
									   basic_async_io_service<Protocol, Executor, SSLService>>
	{
	public:
		using endpoint_type = boost::asio::ip::basic_endpoint<Protocol>;

		using execution_base_type =
			boost::asio::detail::execution_context_service_base<basic_async_io_service<Protocol, Executor, SSLService>>;

		using executor_type = Executor;

		using socket_type = typename SSLService::socket_type;

		using protocol_type = Protocol;

		struct implementation_type
		{
			socket_type* socket;
		};

	public:
		explicit basic_async_io_service(const executor_type& ex)
			: execution_base_type(ex)
		{}

		basic_async_io_service(boost::asio::execution_context& context)
			: execution_base_type(context)
		{}

		virtual ~basic_async_io_service() = default;

	public:
		void construct(implementation_type& /*impl*/)
		{
			// this->base_construct(impl);
		}

		void move_copy(implementation_type& impl, socket_type socket)
		{
			impl.socket = new socket_type(std::move(socket));

			impl_ptr_ = std::make_shared<SSLService>(*impl.socket);
		}

		void destroy(implementation_type& impl)
		{
			if (!impl.socket->is_open())
				return;

			error_code ec{};

			if (impl.socket)
			{
				impl.socket->shutdown(boost::asio::socket_base::shutdown_both, ec);

				impl.socket->close(ec);

				delete impl.socket;
			}
		}

		virtual void shutdown() override
		{
			return;
		}

		auto async_connect(const std::string& ip_addr, const std::string& port, error_code& ec)
			-> boost::asio::awaitable<void>
		{
			co_return co_await impl_ptr_->async_connect(ip_addr, port, ec);
		}

		auto async_read_some(flex_buffer_t& buffer, error_code& ec) -> boost::asio::awaitable<std::size_t>
		{
			co_return co_await impl_ptr_->async_read_some(buffer, ec);
		}

		auto async_write_some(flex_buffer_t buffer, error_code& ec) -> boost::asio::awaitable<std::size_t>
		{
			co_return co_await impl_ptr_->async_write_some(buffer, ec);
		}

		auto start() -> boost::asio::awaitable<void>
		{
			XLOG_INFO() << "start success at " << remote_address() << ":" << remote_port()
						<< ", async read establish";

			if constexpr (is_startable<SSLService>)
			{
				co_await impl_ptr_->start();
			}

			this->keep_alive(true);

			this->set_nodelay(true);

			co_return;
		}

		bool keep_alive(bool value)
		{
			error_code ec;

			impl_ptr_->get_implementation().set_option(boost::asio::socket_base::keep_alive(value), ec);

			XLOG_INFO() << "set keep alive :" << value;

			return !ec;
		}

		bool set_nodelay(bool enable)
		{
			error_code ec;
			impl_ptr_->get_implementation().set_option(boost::asio::ip::tcp::no_delay(enable), ec);

			XLOG_INFO() << "set nodelay :" << enable;

			return !ec;
		}

		std::string remote_address() const
		{
			return impl_ptr_->get_implementation().remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u() const
		{
			return impl_ptr_->get_implementation().remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port() const
		{
			return impl_ptr_->get_implementation().remote_endpoint().port();
		}

	private:
		std::shared_ptr<SSLService> impl_ptr_;
	};
} // namespace aquarius