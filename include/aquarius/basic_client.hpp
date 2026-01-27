#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/http_config_schedule.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/header_fields.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <functional>

namespace aquarius
{
	using namespace std::chrono_literals;

	template <typename Session, typename Executor = any_io_executor>
	class basic_client : public std::enable_shared_from_this<basic_client<Session>>
	{
		using socket = Session::socket;

		using resolver = Session::resolver;

	public:
		explicit basic_client(io_context& context, std::chrono::milliseconds timeout = 1000ms)
			: basic_client(context.get_executor(), timeout)
		{}

		basic_client(const Executor& context, std::chrono::milliseconds timeout = 1000ms)
			: executor_(context)
			, host_()
			, port_()
			, session_ptr_(nullptr)
			, close_func_()
			, timeout_(timeout)
		{}

		virtual ~basic_client() = default;

	public:
		auto async_connect(const std::string& host, const std::string& port) -> awaitable<bool>
		{
			host_ = host;
			port_ = port;

			session_ptr_ = std::make_shared<Session>(std::move(socket(executor_)));

			co_return co_await session_ptr_->async_connect(host_, port_);
		}

		auto reconnect() -> awaitable<error_code>
		{
			co_return co_await async_connect(host_, port_);
		}

		template <typename Response, typename Request>
		auto async_send(std::shared_ptr<Request> req) -> awaitable<Response>
		{
			flex_buffer buffer{};

			req->seq_number(static_cast<uint32_t>(detail::uuid_generator()()));

			req->commit(buffer);

			error_code ec{};

			virgo::header_fields hf;

			auto buf = co_await async_send(buffer, ec, req->seq_number(), hf);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.what();
				}
				session_ptr_->shutdown();
				if (close_func_)
					close_func_();

				co_return Response{};
			}

			if constexpr (Session::tag == proto_tag::http)
			{
				Response resp{ *std::dynamic_pointer_cast<virgo::header_fields>(hf) };
				resp.consume(buf);
				co_return resp;
			}
			else
			{
				Response resp{ *hf};
				resp.consume(buf);

				co_return resp;
			}
		}

		auto async_send(flex_buffer& buffer, error_code& ec, std::size_t id, virgo::header_fields& hf)
			-> awaitable<flex_buffer>
		{
			ec = co_await session_ptr_->async_send(buffer);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.what();
				}
				session_ptr_->shutdown();
				if (close_func_)
					close_func_();

				co_return flex_buffer{};
			}

			co_return co_await session_ptr_->query_buffer(id, hf);
		}

		std::string remote_address() const
		{
			return session_ptr_->remote_address();
		}

		int remote_port() const
		{
			return session_ptr_->remote_port();
		}

		template <typename Func>
		void set_close_func(Func&& f)
		{
			close_func_ = std::forward<Func>(f);
		}

		void close()
		{
			session_ptr_->close();

			std::shared_ptr<Session>().swap(session_ptr_);
		}

	private:
		Executor executor_;

		std::string host_;

		std::string port_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void()> close_func_;

		std::chrono::milliseconds timeout_;
	};
} // namespace aquarius