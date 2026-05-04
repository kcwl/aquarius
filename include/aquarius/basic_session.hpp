#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <expected>

namespace aquarius
{
	template <typename Protocol, template<typename>typename Adaptor>
	class basic_session : public std::enable_shared_from_this<basic_session<Protocol, Adaptor>>
	{
	public:
		using socket = typename Protocol::socket;

		using endpoint = typename Protocol::endpoint;

		using acceptor = typename Protocol::acceptor;

		using resolver = typename Protocol::resolver;

		using duration = std::chrono::system_clock::duration;

		using adaptor_t = Adaptor<socket>;

	public:
		explicit basic_session(socket _socket, duration timeout)
			: socket_(std::move(_socket))
			, timeout_(timeout)
			, socket_adaptor_(socket_)
			, uuid_(detail::uuid_generator()())
			, proto_(get_executor())
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

		auto async_connect(const std::string& host, const std::string& port) -> asio::awaitable<error_code>
		{
			resolver resolve(this->get_executor());

			auto ec = co_await socket_adaptor_.async_connect(resolve.resolve(host, port), timeout_);

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

		auto async_read(flex_buffer& buffer, std::size_t length) -> asio::awaitable<error_code>
		{
			error_code ec;

			auto mutable_buffer = buffer.prepare(length);

			co_await asio::async_read(socket_adaptor_.get_implement(), mutable_buffer,
									  asio::redirect_error(asio::use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "[async read] error: " << ec.what() << ", remote_address: " << remote_address();
			}
			else
			{
				XLOG_DEBUG() << "[async read] receive " << length << " bytes";

				buffer.commit(length);
			}

			co_return ec;
		}

		auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& end_pos) -> asio::awaitable<error_code>
		{
			error_code ec;

			end_pos = co_await boost::asio::async_read_until(socket_adaptor_.get_implement(), flex_buffer_ref(buffer),
															   delm, asio::redirect_error(asio::use_awaitable, ec));

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

		auto async_send(flex_buffer&& buffer) -> asio::awaitable<error_code>
		{
			error_code ec{};

			co_await socket_adaptor_.get_implement().async_write_some(buffer.data(),
																	  asio::redirect_error(asio::use_awaitable, ec));

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

		auto async_send(flex_buffer_view buffer_view) -> asio::awaitable<error_code>
		{
			error_code ec{};

			co_await socket_adaptor_.get_implement().async_write_some(asio::buffer(buffer_view.data(), buffer_view.size()),
																	  asio::redirect_error(asio::use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "[async send view] error: " << ec.what();
			}
			else
			{
				XLOG_DEBUG() << "[async send view] send " << buffer_view.size() << " bytes";
			}

			co_return ec;
		}

		bool close()
		{
			error_code ec;

			socket_.shutdown(asio::socket_base::shutdown_both, ec);

			if (ec)
			{
				XLOG_ERROR() << "[shutdown] error: " << ec.what();
			}

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

			socket_.set_option(typename Protocol::keep_alive(enable), ec);

			if (ec)
			{
				XLOG_ERROR() << "[keep alive] error: " << ec.what();
			}

			return !ec;
		}

		bool set_nodelay(bool enable = true)
		{
			error_code ec;

			socket_.set_option(typename Protocol::no_delay(enable), ec);

			if (ec)
			{
				XLOG_ERROR() << "[set nodelay] error: " << ec.what();
			}

			return !ec;
		}

		auto accept() -> asio::awaitable<error_code>
		{
			auto ec = co_await this->socket_adaptor_.async_handshake(timeout_);

			if (ec)
			{
				co_return ec;
			}

			co_return co_await proto_.accept(this->shared_from_this());
		}

		auto query() -> asio::awaitable<error_code>
		{
			co_return co_await proto_.query(this->shared_from_this());
		}

		template<typename Func>
		auto wait(std::size_t src, Func&& f)
		{
			return proto_.wait(src, std::forward<Func>(f));
		}

		template<typename Request>
		bool make_request(std::shared_ptr<Request> request, flex_buffer& buffer)
		{
			return proto_.make_request(request, buffer);
		}

	protected:
		socket socket_;

		adaptor_t socket_adaptor_;

		std::size_t uuid_;

		duration timeout_;

		Protocol proto_;
	};
} // namespace aquarius