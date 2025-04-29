#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/session_object_impl.hpp>
#include <aquarius/core/uuid.hpp>

namespace aquarius
{
	template <typename IO>
	class basic_session : public std::enable_shared_from_this<basic_session<IO>>
	{
		using executor_type = typename IO::executor_type;

		using socket_type = typename IO::socket_type;

		using ssl_socket_type = typename IO::ssl_socket_type;

		using this_type = basic_session<IO>;

		constexpr static std::size_t pack_limit = 4096;

	public:
		explicit basic_session(socket_type socket)
			: impl_(std::move(socket))
			, read_buffer_()
			, uuid_(invoke_uuid<uint32_t>())
		{}

		template <execution_context_convertible ExecutionContext>
		explicit basic_session(ExecutionContext& context)
			: impl_(context)
			, read_buffer_(pack_limit)
			, uuid_(invoke_uuid<uint32_t>())
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
			// invoke_session_helper::push(this->shared_from_this());

			co_await impl_.get_service().start(impl_.get_implementation());

			co_await read_messages();
		}

		void send_packet(flex_buffer_t&& buffer)
		{
			//pack_.async_write(std::move(buffer));
		}

		void shutdown()
		{
			return impl_.get_service().shutdown();
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

	private:
		auto read_messages() -> boost::asio::awaitable<void>
		{
			//read_buffer_.normalize();

			//read_buffer_.ensure();

			auto self(this->shared_from_this());

			boost::system::error_code ec;

			std::size_t bytes_transferred =
				co_await impl_.get_service().async_read_some(impl_.get_implementation(), boost::asio::buffer(read_buffer_), ec);

			if (ec)
			{
				if (ec != asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}

				// invoke_session_helper::erase(this->uuid());

				co_return impl_.get_service().shutdown();
			}

			//read_buffer_.commit(static_cast<int>(bytes_transferred));

			co_await read_messages();
		}

	private:
		session_object_impl<IO, executor_type, IO::ssl_version> impl_;

		std::array<uint8_t, pack_limit> read_buffer_;

		uint32_t uuid_;
	};
} // namespace aquarius