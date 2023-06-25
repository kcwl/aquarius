#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/deadline_timer.hpp>
#include <aquarius/detail/noncopyable.hpp>
#include <aquarius/detail/transfer.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <queue>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace detail
	{
		class basic_context;
	}
} // namespace aquarius

namespace aquarius
{
	template <typename _ConnectType, typename _SocketType = void>
	class connect : private detail::noncopyable, public std::enable_shared_from_this<connect<_ConnectType, _SocketType>>
	{
		using socket_t = boost::asio::ip::tcp::socket;

		using ssl_socket_t = boost::asio::ssl::stream<socket_t&>;

		using ssl_context_t = boost::asio::ssl::context;

	public:
		explicit connect(boost::asio::io_service& io_service, std::chrono::steady_clock::duration dura = heart_time_interval)
			: socket_(io_service)
			, ssl_context_(ssl_context_t::sslv23)
			, ssl_socket_(socket_, ssl_context_)
			, read_buffer_()
			, write_queue_()
			, connect_timer_(io_service)
			, connect_time_()
			, dura_(dura)
			, uid_()
		{
			init_context();

			boost::uuids::random_generator_mt19937 generator{};

			uid_ = generator();
		}

		virtual ~connect()
		{
			shut_down();
		}

		auto& socket()
		{
			return socket_;
		}

		std::string remote_address()
		{
			return socket_.remote_endpoint().address().to_string();
		}

		uint32_t remote_address_u()
		{
			return socket_.remote_endpoint().address().to_v4().to_uint();
		}

		uint16_t remote_port()
		{
			return socket_.remote_endpoint().port();
		}

		void async_write(flex_buffer_t&& resp_buf)
		{
			write_queue_.push(std::forward<flex_buffer_t>(resp_buf));

			async_process_queue();
		}

		void async_read()
		{
			if (!socket_.is_open())
				return;

			read_buffer_.normalize();
			read_buffer_.ensure();

			if constexpr (std::same_as<_SocketType, ssl_socket>)
			{
				ssl_socket_.async_read_some(boost::asio::buffer(read_buffer_.rdata(), read_buffer_.active()),
											std::bind(&connect::read_handle, this->shared_from_this(),
													  std::placeholders::_1, std::placeholders::_2));
			}
			else
			{
				socket_.async_read_some(boost::asio::buffer(read_buffer_.wdata(), read_buffer_.active()),
										std::bind(&connect::read_handle, this->shared_from_this(),
												  std::placeholders::_1, std::placeholders::_2));
			}
		}

		void start()
		{
			if constexpr (std::same_as<_SocketType, ssl_socket>)
			{
				ssl_socket_.async_handshake(boost::asio::ssl::stream_base::server,
											[](const boost::system::error_code& ec)
											{
												if (ec)
													return;

												establish_async_read();
											});
			}
			else
			{
				establish_async_read();
			}
		}

		virtual void shut_down()
		{
			if (socket_.is_open())
			{
				boost::system::error_code ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			}

			connect_timer_.cancel();

			socket_.close();

			if (on_close_)
				on_close_();
		}

		virtual std::size_t uuid()
		{
			return boost::uuids::hash_value(uid_);
		}

		std::chrono::milliseconds get_connect_duration()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -
																		 connect_time_);
		}

		auto get_connect_time()
		{
			return connect_time_;
		}

		void set_sndbuf_size(int value) noexcept
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::send_buffer_size(value), ec);
		}

		int get_sndbuf_size() noexcept
		{
			boost::asio::socket_base::send_buffer_size option{};

			boost::system::error_code ec;

			socket_.get_option(option, ec);

			return option.value();
		}

		void set_rcvbuf_size(int value) noexcept
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::receive_buffer_size(value), ec);
		}

		int get_rcvbuf_size() noexcept
		{
			boost::asio::socket_base::receive_buffer_size option{};

			boost::system::error_code ec;

			socket_.get_option(option, ec);

			return option.value();
		}

		void keep_alive(bool value) noexcept
		{
			boost::system::error_code ec;

			socket_.set_option(boost::asio::socket_base::keep_alive(value), ec);
		}

		void set_delay(bool enable)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::ip::tcp::no_delay(enable), ec);
		}

		void reuse_address(bool value)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::reuse_address(value), ec);
		}

		void set_linger(bool enable, int timeout)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::linger(enable, timeout), ec);
		}

		template<connect_event e, typename _Func>
		void regist_func(_Func&& f)
		{
			if constexpr (e == connect_event::start)
			{
				on_start_ = std::forward<_Func>(f);
			}
			else if constexpr (e == connect_event::close)
			{
				on_close_ = std::forward<_Func>(f);
			}
		}

	protected:
		void async_process_queue()
		{
			if (write_queue_.empty())
				return;

			auto& buffer = write_queue_.front();

			if constexpr (std::same_as<_SocketType, ssl_socket>)
			{
				ssl_socket_.async_write_some(boost::asio::buffer(buffer.wdata(), buffer.size()),
											 std::bind(&connect::write_handle, this->shared_from_this(),
													   std::placeholders::_1, std::placeholders::_2));
			}
			else
			{
				socket_.async_write_some(boost::asio::buffer(buffer.wdata(), buffer.size()),
										 std::bind(&connect::write_handle, this->shared_from_this(),
												   std::placeholders::_1, std::placeholders::_2));
			}
		}

	private:
		void read_handle(const boost::system::error_code& ec, std::size_t bytes_transferred)
		{
			if (ec)
			{
				return shut_down();
			}

			read_buffer_.commit(static_cast<int>(bytes_transferred));

			auto res = read_handle_internal();

			if (res == read_handle_result::error)
			{
				return shut_down();
			}

			async_read();
		}

		void write_handle(const boost::system::error_code& ec, std::size_t bytes_transferred)
		{
			if (ec)
			{
				std::cout << ec.message() << std::endl;
				shut_down();
				return;
			}

			write_queue_.pop();

			async_process_queue();

			std::cout << "complete " << bytes_transferred << " bytes\n";
		}

		void init_context()
		{
			if constexpr (std::same_as<_SocketType, ssl_socket>)
			{
				ssl_context_.set_options(ssl_context_t::default_workarounds | ssl_context_t::no_sslv2 |
										 ssl_context_t::single_dh_use);

				ssl_context_.set_password_callback([]() { return ""; });
				ssl_context_.use_certificate_chain_file("server.pem");
				ssl_context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
				ssl_context_.use_tmp_dh_file("dh4096.pem");
			}
		}

		void heart_deadline()
		{
			connect_timer_.expires_from_now(dura_);
			connect_timer_.async_wait(
				[this, self = this->shared_from_this()](const boost::system::error_code& ec)
				{
					if (ec == boost::asio::error::operation_aborted)
						return;

					if (!ec)
						return heart_deadline();

					shut_down();
				});
		}

		read_handle_result read_handle_internal()
		{
			if constexpr (std::same_as<_ConnectType, connect_tcp>)
			{
				uint32_t id = 0;

				elastic::from_binary(id, read_buffer_);

				if (id == 0)
				{
					return read_handle_result::waiting_for_query;
				}

				auto ctx_iter = this->ctxs_.find(id);

				std::shared_ptr<detail::basic_context> ctx_ptr;

				if (ctx_iter == this->ctxs_.end())
				{
					ctx_ptr = detail::context_invoke_helper::invoke(id);

					this->ctxs_.insert({ id, ctx_ptr });
				}
				else
				{
					ctx_ptr = ctx_iter->second;
				}

				auto req_ptr = detail::message_invoke_helpter::invoke(id);

				if (!req_ptr)
				{
					return read_handle_result::error;
				}

				auto res = req_ptr->visit(read_buffer_, visit_mode::input);

				if (res != read_handle_result::ok)
				{
					return res;
				}

				auto result = req_ptr->accept(
					ctx_ptr, std::make_shared<detail::transfer>(std::bind(&connect::async_write, this->shared_from_this(), std::placeholders::_1)));

				if (result == 0)
				{
					std::cout << std::format("warning: no ctx for req: {}\n", id);
				}
			}
			else if constexpr (std::same_as<_ConnectType, connect_http>)
			{}
			else
			{}

			return read_handle_result::ok;
		}

		void establish_async_read()
		{
			connect_time_ = std::chrono::system_clock::now();

			if (on_start_)
				on_start_();

			heart_deadline();

			async_read();
		}


	public:
		std::map<uint32_t, std::shared_ptr<detail::basic_context>> ctxs_;

	private:
		socket_t socket_;

		ssl_context_t ssl_context_;

		ssl_socket_t ssl_socket_;

		flex_buffer_t read_buffer_;

		std::queue<flex_buffer_t> write_queue_;

		detail::steady_timer connect_timer_;

		std::chrono::system_clock::time_point connect_time_;

		std::chrono::steady_clock::duration dura_;

		boost::uuids::uuid uid_;

		std::function<void()> on_start_;

		std::function<void()> on_close_;
	};
} // namespace aquarius
