#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/serialize/flex_buffer.hpp>

namespace aquarius
{
	template <auto Tag, typename ProtoTag, typename Adaptor>
	class basic_session
	{
	public:
		using socket = typename protocol<Tag>::socket;

		using endpoint = typename protocol<Tag>::endpoint;

		using acceptor = typename protocol<Tag>::acceptor;

		using resolver = typename protocol<Tag>::resolver;

		using executor_type = typename socket::executor_type;

		using callback_func = std::function<void(std::shared_ptr<basic_session<Tag, ProtoTag, Adaptor>>)>;

		constexpr static auto tag = Tag;

	public:
		explicit basic_session(socket sock)
			: socket_(std::move(sock))
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

		auto async_connect(const std::string& host, const std::string& port) -> awaitable<bool>
		{
			auto ec = co_await socket_adaptor_.async_connect(host, port);

			if (ec)
			{
				XLOG_ERROR() << "async_connect error: " << ec.what();
			}

			co_return !ec;
		}

		std::string remote_address() const
		{
			return socket_.remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u() const
		{
			error_code ec{};

			auto edp = socket_.remote_endpoint(ec);

			if (ec)
			{
				return 0;
			}

			return edp.address().to_v4().to_uint();
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
				XLOG_INFO() << "async_read exception: " << ec.what() << ", remote_address: " << remote_address();
			}
			else
			{
				XLOG_DEBUG() << "[async read] receive " << length << " bytes";
			}

			buffer.commit(length);

			co_return ec;
		}

		auto async_read_util(flex_buffer& buffer, std::string_view delm) -> awaitable<error_code>
		{
			error_code ec;

			co_await boost::asio::async_read_until(socket_adaptor_.get_implement(), flex_buffer_ref(buffer), delm,
												   redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_INFO() << "async_read_util exception: " << ec.what() << ", remote_address: " << remote_address();
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

			if (!ec)
			{
				XLOG_DEBUG() << "[async send] send " << buffer.size() << " bytes";
			}

			co_return ec;
		}

		bool shutdown()
		{
			error_code ec;

			socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);

			return !ec;
		}

		bool close()
		{
			error_code ec;

			socket_.close(ec);

			return !ec;
		}

		bool keep_alive(bool enable = true)
		{
			error_code ec;

			socket_.set_option(typename protocol<Tag>::keep_alive(enable), ec);

			return !ec;
		}

		bool set_nodelay(bool enable = true)
		{
			error_code ec;

			socket_.set_option(typename protocol<Tag>::no_delay(enable), ec);

			return !ec;
		}

	protected:
		socket socket_;

		Adaptor socket_adaptor_;

		std::size_t uuid_;

		callback_func close_func_;
	};
} // namespace aquarius