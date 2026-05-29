#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <expected>
#include <map>

namespace aquarius
{
	template <typename Protocol, template <typename> typename Adaptor>
	class basic_session : public std::enable_shared_from_this<basic_session<Protocol, Adaptor>>
	{
	public:
		using socket = typename Protocol::socket;

		using endpoint = typename Protocol::endpoint;

		using acceptor = typename Protocol::acceptor;

		using resolver = typename Protocol::resolver;

		using duration = std::chrono::system_clock::duration;

		using adaptor_t = Adaptor<socket>;

		struct callback
		{
			using func_t = std::function<void(flex_buffer&)>;

			func_t func;

			bool complete;
		};

	public:
		explicit basic_session(socket _socket, duration timeout)
			: socket_(std::move(_socket))
			, timeout_(timeout)
			, socket_adaptor_(socket_)
			, uuid_(detail::uuid_generator()())
			, proto_()
			, buffer_channel_(get_executor())
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

		auto async_read_util(flex_buffer& buffer, std::string_view delm, std::size_t& end_pos)
			-> asio::awaitable<error_code>
		{
			error_code ec;

			end_pos = co_await boost::asio::async_read_until(socket_adaptor_.get_implement(), buffer, delm,
															 asio::redirect_error(asio::use_awaitable, ec));

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

		template <typename ConstBufferSequence>
		auto async_send(ConstBufferSequence&& buffers) -> asio::awaitable<error_code>
		{
			error_code ec{};

			co_await asio::async_write(socket_adaptor_.get_implement(), buffers,
									   asio::redirect_error(asio::use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "[async send] error: " << ec.what();
			}
			else
			{
				XLOG_DEBUG() << "[async send] send buffers:" << buffers.size();
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

		auto wait(std::size_t src) -> asio::awaitable<void>
		{
			for (;;)
			{
				auto next_src = co_await buffer_channel_.async_receive(asio::use_awaitable);

				if (next_src == src)
				{
					break;
				}
			}
		}

		template <typename Request, typename Func>
		auto send_request(std::shared_ptr<Request> request, Func&& func, error_code& ec) -> asio::awaitable<std::size_t>
		{
			co_return co_await proto_.send_request(this->shared_from_this(), request, std::forward<Func>(func), ec);
		}

		auto send_buffer(flex_buffer& req, flex_buffer& resp, error_code& ec) -> asio::awaitable<std::size_t>
		{
			co_return co_await proto_.send_buffer(this->shared_from_this(), req, resp, ec);
		}

		bool filling_buffer(std::size_t src, flex_buffer& buffer)
		{
			auto iter = buffers_.find(src);

			if (iter == buffers_.end())
			{
				buffer_channel_.try_send(error_code{}, 0);

				return false;
			}

			if (iter->second->func)
			{
				iter->second->func(buffer);
			}

			iter->second->complete = true;

			buffer_channel_.try_send(error_code{}, src);

			return true;
		}

		template <typename Func>
		void regist_resp_func(std::size_t src, Func&& func)
		{
			auto& cb = buffers_[src];
			if (!cb)
			{
				cb = std::make_shared<callback>();
			}

			cb->func = func;
		}

	protected:
		socket socket_;

		adaptor_t socket_adaptor_;

		std::size_t uuid_;

		duration timeout_;

		Protocol proto_;

		asio::experimental::channel<void(error_code, std::size_t)> buffer_channel_;

		std::map<std::size_t, std::shared_ptr<callback>> buffers_;
	};
} // namespace aquarius