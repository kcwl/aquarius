#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <auto Tag, template<bool,typename,typename>typename Adaptor>
	class basic_session
	{
	public:
		using socket = typename protocol<Tag>::socket;

		using endpoint = typename protocol<Tag>::endpoint;

		using acceptor = typename protocol<Tag>::acceptor;

		using resolver = typename protocol<Tag>::resolver;

		using duration = std::chrono::system_clock::duration;

		using adaptor_t = Adaptor<true, socket, resolver>;

	public:
		explicit basic_session(socket _socket, duration timeout)
			: socket_(std::move(_socket))
			, timeout_(timeout)
			, socket_adaptor_(socket_)
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

		auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code>
		{
			auto ec = co_await socket_adaptor_.async_connect(host, port, timeout_);

			if (ec)
			{
				XLOG_ERROR() << "async_connect error: " << ec.what();
			}

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

		auto async_read(flex_buffer& buffer, std::size_t length) -> awaitable<error_code>
		{
			error_code ec;

			auto mutable_buffer = buffer.prepare(length);

			co_await boost::asio::async_read(socket_adaptor_.get_implement(), mutable_buffer,
									  redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "[async read] error: " << ec.what() << ", remote_address: " << remote_address();
			}
			else
			{
				buffer.commit(length);

				XLOG_DEBUG() << "[async read] receive " << length << " bytes";
			}

			co_return ec;
		}

		auto async_read_util(flex_buffer& buffer, std::string_view delm) -> awaitable<error_code>
		{
			error_code ec;

			co_await boost::asio::async_read_until(socket_adaptor_.get_implement(), flex_buffer_ref(buffer), delm,
												   redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "[async read util] error: " << ec.what() << ", remote_address: " << remote_address();
			}
			else
			{
				XLOG_DEBUG() << "[async read util] receive " << buffer.size() << " bytes";
			}

			co_return ec;
		}

		auto async_send(const flex_buffer& buffer) -> awaitable<error_code>
		{
			error_code ec{};

			co_await socket_adaptor_.get_implement().async_write_some(buffer.data(), redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "[async send] error: " << ec.what();
			}
			else
			{
				XLOG_DEBUG() << "[async send] send " << buffer.size() << " bytes";
			}

			co_return ec;
		}

		bool shutdown()
		{
			error_code ec;

			socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);

			if (ec)
			{
				XLOG_ERROR() << "[shutdown] error: " << ec.what();
			}

			return !ec;
		}

		bool close()
		{
			error_code ec;

			socket_.close(ec);

			if (ec)
			{
				XLOG_ERROR() << "[close] error: " << ec.what();
			}

			return !ec;
		}

		bool keep_alive(bool enable = true)
		{
			error_code ec;

			socket_.set_option(typename protocol<Tag>::keep_alive(enable), ec);

			if (ec)
			{
				XLOG_ERROR() << "[keep alive] error: " << ec.what();
			}

			return !ec;
		}

		bool set_nodelay(bool enable = true)
		{
			error_code ec;

			socket_.set_option(typename protocol<Tag>::no_delay(enable), ec);

			if (ec)
			{
				XLOG_ERROR() << "[set nodelay] error: " << ec.what();
			}

			return !ec;
		}

	protected:
		socket socket_;

		adaptor_t socket_adaptor_;

		std::size_t uuid_;

		duration timeout_;
	};
} // namespace aquarius