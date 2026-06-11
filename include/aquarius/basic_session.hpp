#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <map>

namespace aquarius
{
	template <typename Protocol>
	class basic_session
	{
	public:
		using socket_type = typename Protocol::socket_type;

		using resolver_type = typename Protocol::resolver_type;

		using duration = std::chrono::system_clock::duration;

	public:
		explicit basic_session(socket_type _socket, duration timeout)
			: socket_(std::move(_socket))
			, timeout_(timeout)
			, uuid_(detail::uuid_generator()())
		{}

		virtual ~basic_session() = default;

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

			co_await asio::async_connect(socket_, resolve.resolve(host, port),
										 asio::cancel_after(timeout_, asio::redirect_error(asio::use_awaitable, ec)));

			co_return ec;
		}

		std::string remote_address() const
		{
			return socket_.remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u() const
		{
			return socket_.remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port() const
		{
			return socket_.remote_endpoint().port();
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

			socket_.shutdown(asio::socket_base::shutdown_both, ec);

			if (ec)
			{
				XLOG_ERROR() << "[shutdown] error: " << ec.what();
			}

			return !ec;
		}

		error_code keep_alive(bool enable)
		{
			error_code ec;

			socket_.set_option(typename Protocol::keep_alive(enable), ec);

			return ec;
		}

		error_code set_nodelay(bool enable)
		{
			error_code ec;

			socket_.set_option(typename Protocol::no_delay(enable), ec);

			return ec;
		}

	protected:
		socket_type socket_;

		std::size_t uuid_;

		duration timeout_;
	};
} // namespace aquarius