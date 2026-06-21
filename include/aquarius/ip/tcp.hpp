#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_server.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/context_reg.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/serialize/binary.hpp>
#include <expected>
#include <type_traits>

namespace aquarius
{
	class tcp : public std::enable_shared_from_this<tcp>
	{
	public:
		using socket = asio::ip::tcp::socket;

		using endpoint = asio::ip::tcp::endpoint;

		using acceptor = asio::ip::tcp::acceptor;

		using resolver = asio::ip::tcp::resolver;

		using no_delay = asio::ip::tcp::no_delay;

		using keep_alive = asio::socket_base::keep_alive;

		using session_type = basic_session<tcp>;

		using duration = typename session_type::duration;

		template <typename Handler>
		using context = basic_context<Handler, tcp>;

		using client = basic_client<tcp>;

		using server = basic_server<tcp>;

		struct raw_header
		{
			uint32_t length;
			uint32_t src;
		};

		struct callback
		{
			using func_t = std::function<asio::awaitable<error_code>(flex_buffer&, const std::string&)>;

			func_t func;

			bool complete;
		};

	public:
		tcp(socket socket, duration timeout)
			: session_ptr_(std::make_shared<session_type>(std::move(socket), timeout))
			, buffer_channel_(socket.get_executor())
		{}

	public:
		auto uuid() const
		{
			return session_ptr_->uuid();
		}

		auto async_connect(const std::string& host, const std::string& port) -> asio::awaitable<error_code>
		{
			co_return co_await session_ptr_->async_connect(host, port);
		}

		auto accept() -> asio::awaitable<error_code>
		{
			error_code ec{};

			for (;;)
			{
				flex_buffer buffer{};
				uint32_t src{};
				ec = co_await recv(buffer, src);

				if (ec)
				{
					break;
				}

				auto router = binary_parse{}.from_datas<std::string>(buffer);

				XLOG_INFO() << "[accept] parse protocol router: " << router;

				auto context = mpc_get_context(router);

				if (!context)
				{
					continue;
				}

				auto ptr = std::dynamic_pointer_cast<basic_protocol_context<tcp>>(context);
				if (!ptr)
				{
					continue;
				}

				ptr->attach_router(router);

				ptr->visit(buffer);

				auto self = this->shared_from_this();

				asio::co_spawn(
					session_ptr_->get_executor(),
					[ptr, src, this, self]() -> asio::awaitable<void>
					{
						auto ec = co_await ptr->complete(this,
														 [this, src](flex_buffer& buffer) -> asio::awaitable<error_code>
														 {
															 raw_header header{};
															 header.src = src;
															 header.length = static_cast<uint32_t>(buffer.size());

															 std::vector<asio::const_buffer> buffers{};
															 commit_raw_header(buffers, header);

															 buffers.push_back(buffer.data());

															 co_return co_await session_ptr_->async_send(buffers);
														 });

						if (ec)
						{
							XLOG_ERROR() << "[mpc_publish] publish error:" << ec.what();
						}
					},
					asio::detached);
			}

			if (ec != asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		auto query() -> asio::awaitable<error_code>
		{
			error_code ec{};

			for (;;)
			{
				flex_buffer buffer{};
				uint32_t src{};
				ec = co_await recv(buffer, src);
				if (ec)
				{
					break;
				}

				auto router = binary_parse{}.from_datas<std::string>(buffer);

				XLOG_INFO() << "[query] parse protocol router: " << router;

				if (!co_await filling_buffer(src, buffer, router))
				{
					auto context = mpc_get_context(router);

					if (!context)
					{
						continue;
					}

					auto ptr = std::dynamic_pointer_cast<basic_protocol_context<tcp>>(context);

					if (!ptr)
					{
						continue;
					}

					ptr->visit(buffer);

					auto self = this->shared_from_this();

					asio::co_spawn(
						session_ptr_->get_executor(), [this, self, ptr]() -> asio::awaitable<void>
						{
							co_await ptr->complete(this, {});
						}, asio::detached);
				}
			}

			if (ec != asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		error_code keepalive(bool enable)
		{
			return session_ptr_->keep_alive(enable);
		}

		error_code nodelay(bool enable)
		{
			return session_ptr_->set_nodelay(enable);
		}

		bool shutdown()
		{
			return session_ptr_->shutdown();
		}

		template <typename Request, typename Func>
		auto send_request(std::shared_ptr<Request> request, Func&& func, error_code& ec) -> asio::awaitable<std::size_t>
		{
			flex_buffer buffer{};
			request->commit(buffer);

			raw_header header{};
			header.src = detail::uuid_generator()();

			header.length = static_cast<uint32_t>(buffer.size());

			std::vector<asio::const_buffer> buffers{};

			commit_raw_header(buffers, header);

			regist_resp_func(header.src, func);

			buffers.push_back(buffer.data());

			ec = co_await session_ptr_->async_send(buffers);

			co_return header.src;
		}

		template <typename Func>
		auto send_buffer(flex_buffer& req, const std::string& router, Func&& f, error_code& ec) -> asio::awaitable<std::size_t>
		{
			raw_header header{};
			header.src = detail::uuid_generator()();
			header.length = static_cast<uint32_t>(req.size());

			regist_resp_func(header.src, std::forward<Func>(f));

			flex_buffer router_buffer{};
			if (!router.empty())
			{
				binary_parse{}.to_datas(router, router_buffer);
				header.length += static_cast<uint32_t>(router_buffer.size());
			}

			std::vector<asio::const_buffer> buffers{};

			commit_raw_header(buffers, header);
			if (!router.empty())
			{
				buffers.push_back(router_buffer.data());
			}
			
			buffers.push_back(req.data());

			ec = co_await session_ptr_->async_send(buffers);

			co_return header.src;
		}

		template <typename Handler, typename Func>
		auto handle_request(std::shared_ptr<Handler> handler_ptr, Func&& func) -> asio::awaitable<error_code>
		{
			handler_ptr->attach_session(func);

			auto ec = co_await handler_ptr->handle();

			if (ec)
			{
				co_return ec;
			}

			flex_buffer resp_buffer{};

			handler_ptr->response().commit(resp_buffer);

			co_return co_await func(resp_buffer);
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

	private:
		auto recv(flex_buffer& buffer, uint32_t& src) -> asio::awaitable<error_code>
		{
			if (!session_ptr_)
			{
				co_return asio::error::eof;
			}

			raw_header header{};

			constexpr auto len = sizeof(raw_header);

			src = 0;

			auto ec = co_await session_ptr_->async_read(buffer, len);

			if (!ec)
			{
				consume_raw_header(buffer, header);

				src = header.src;

				if (header.length == 0)
				{
					ec = asio::error::eof;
				}
				else
				{
					ec = co_await session_ptr_->async_read(buffer, header.length);
				}
			}

			co_return ec;
		}

		void consume_raw_header(flex_buffer& buffer, raw_header& header)
		{
			buffer.sgetn((char*)&header, buffer.size());
		}

		void commit_raw_header(std::vector<asio::const_buffer>& buffers, const raw_header& header)
		{
			buffers.push_back(asio::const_buffer((char*)&header, sizeof(header)));
		}

		auto filling_buffer(std::size_t src, flex_buffer& buffer, const std::string& router) -> asio::awaitable<bool>
		{
			auto iter = buffers_.find(src);

			if (iter == buffers_.end())
			{
				buffer_channel_.try_send(error_code{}, 0);

				co_return false;
			}

			if (iter->second->func)
			{
				co_await iter->second->func(buffer, router);
			}

			iter->second->complete = true;

			buffers_.erase(iter);

			buffer_channel_.try_send(error_code{}, src);

			co_return true;
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

	private:
		asio::experimental::channel<void(error_code, std::size_t)> buffer_channel_;

		std::map<std::size_t, std::shared_ptr<callback>> buffers_;

		std::shared_ptr<session_type> session_ptr_;
	};
} // namespace aquarius