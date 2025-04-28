#pragma once
#include <aquarius/detail/session_object_impl.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/connect/packet.hpp>

namespace aquarius
{
	template<typename IO>
	class basic_session : public std::enable_shared_from_this<basic_session<IO>>
	{
		using executor_type = typename IO::executor_type;

		using socket_type = typename IO::socket_type;

		using this_type = basic_session<IO>;

	public:
		explicit basic_session(socket_type socket)
			: impl_(std::move(socket))
			, read_buffer_(pack_limit)
			, uuid_(invoke_uuid<uint32_t>())
			, pack_(this)
		{

		}

		template<execution_context_convertible ExecutionContext>
		explicit basic_session(ExecutionContext& context)
			: impl_(context)
			, read_buffer_(pack_limit)
			, uuid_(invoke_uuid<uint32_t>())
			, pack_(this)
		{

		}

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

		void start()
		{
			//invoke_session_helper::push(this->shared_from_this());

			impl_.get_service().start(impl_.get_implementation());

			read_messages();
		}

		void send_packet(flex_buffer_t&& buffer)
		{
			pack_.async_write(std::move(buffer));
		}

		void shutdown()
		{
			return impl_.get_service().shutdown();
		}

		void close(bool shutdown)
		{
			return impl_.get_service().close(impl_.get_implementation(), shutdown);
		}

		std::size_t uuid() const
		{
			return uuid_;
		}

		session_object_impl<IO>& get_implemention()
		{
			return impl_;
		}

	private:
		void read_messages()
		{
			read_buffer_.normalize();

			read_buffer_.ensure();

			auto self(this->shared_from_this());

			auto buffer = boost::asio::buffer(read_buffer_.rdata(), read_buffer_.active());

			boost::system::error_code ec;

			std::size_t bytes_transferred{};

			impl_.get_service().read(impl_.get_implementation(), buffer, bytes_transferred, ec);

			if (ec)
			{
				if (ec != asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}

				//invoke_session_helper::erase(this->uuid());

				return impl_.get_service().shutdown();
			}

			read_buffer_.commit(static_cast<int>(bytes_transferred));

			pack_.process(read_buffer_);

			read_messages();
		}

	private:
		session_object_impl<IO> impl_;

		flex_buffer_t read_buffer_;

		uint32_t uuid_;

		packet<this_type> pack_;
	};
}