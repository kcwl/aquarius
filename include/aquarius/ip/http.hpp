#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_context.hpp>
#include <aquarius/basic_server.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/asio.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/context_reg.hpp>
#include <aquarius/ip/http/http_options_handler.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <boost/url.hpp>
#include <format>
#include <ranges>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	class http : public std::enable_shared_from_this<http>
	{
	public:
		using socket = asio::ip::tcp::socket;

		using endpoint = asio::ip::tcp::endpoint;

		using acceptor = asio::ip::tcp::acceptor;

		using resolver = asio::ip::tcp::resolver;

		using no_delay = asio::ip::tcp::no_delay;

		using keep_alive = asio::socket_base::keep_alive;

		constexpr static auto two_crlf = "\r\n\r\n"sv;

		constexpr static auto crlf = "\r\n"sv;

		constexpr static std::size_t header_part = 3;

		using session_type = basic_session<http>;

		using client = basic_client<http>;
		using server = basic_server<http>;

		template <typename Handler>
		using context = basic_context<Handler, http, http_method>;

		using session_callback = std::function<asio::awaitable<error_code>(flex_buffer&, int)>;

		using duration = typename session_type::duration;

		struct callback
		{
			using func_t = std::function<asio::awaitable<error_code>(flex_buffer&, const std::string&)>;

			func_t func;

			bool complete;
		};

	public:
		http(socket socket, duration timeout)
			: session_ptr_(std::make_shared<session_type>(std::move(socket), timeout))
			, buffer_channel_(socket.get_executor())
		{}

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

				std::size_t header_line_end_pos{};

				ec = co_await session_ptr_->async_read_until(buffer, two_crlf, header_line_end_pos);

				if (ec)
				{
					break;
				}

				auto header_line = std::string_view((char*)buffer.data().data(), header_line_end_pos);

				header_line = header_line.substr(0, header_line.find_first_of(crlf));

				buffer.consume(header_line.size() + crlf.size());

				auto [method, url, version] = parse_command_line<true>(header_line, ec);

				if (ec)
				{
					co_await make_error_response(ec);
					continue;
				}

				if (method == http_method::options)
				{
					ec = co_await mpc_http_options(session_ptr_, buffer);
				}
				else
				{
					if (method == http_method::get)
					{
						buffer.sputn(url->params().buffer().data(), url->params().buffer().size());
					}

					std::string router(std::string_view(url->path().data(), url->path().size()));

					ec = co_await recv(buffer);

					std::string_view header_str((char*)buffer.data().data(), buffer.size());

					auto context = mpc_get_context(router);

					if (!context)
					{
						continue;
					}

					auto ptr = std::dynamic_pointer_cast<basic_protocol_context<http, http_method>>(context);
					if (!ptr)
					{
						continue;
					}

					ptr->attach_router(router);

					ptr->visit(buffer);

					auto self = this->shared_from_this();

					ec =
						co_await ptr->complete(this, [this, self, version](flex_buffer& buffer, int result) -> asio::awaitable<error_code>
											   { 
												   std::string raw_header = std::format("{} {} {}", version_to_string(version), result, status_to_string(result));

												   std::vector<asio::const_buffer> buffers{};
												   commit_raw_header(buffers, raw_header);

												   buffers.push_back(buffer.data());

												   co_return co_await session_ptr_->async_send(buffer); }, std::move(method));

					if (ec.value() != static_cast<int>(http_status::ok))
					{
						XLOG_ERROR() << "[mpc_publish] publish error:" << ec.what();
					}
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		auto query() -> asio::awaitable<error_code>
		{
			for (;;)
			{
				flex_buffer buffer{};

				std::size_t end_pos{};

				auto ec = co_await session_ptr_->async_read_until(buffer, two_crlf, end_pos);

				if (ec)
				{
					co_return ec;
				}

				ec = co_await recv(buffer);

				if (ec)
				{
					XLOG_ERROR() << "[query buffer] error: " << ec.what();
					co_return ec;
				}

				std::string_view header_line((char*)buffer.data().data(), buffer.size());
				auto pos = header_line.find_first_of(crlf);

				auto [version, status] = parse_command_line<false>(header_line.substr(0, pos), ec);

				if (ec)
				{
					co_return ec;
				}

				if (status != http_status::ok)
				{
					co_return status;
				}

				buffer.consume(pos);

				std::string_view header_str((char*)buffer.data().data(), buffer.size());

				auto src = parse_key(header_str, "source-seq");

				if (src.empty())
				{
					co_return ec;
				}

				if (!co_await filling_buffer(buffer))
				{
				}

				co_return ec;
			}
		}

		auto recv(flex_buffer& buffer) -> asio::awaitable<error_code>
		{
			error_code ec{};

			std::size_t content_length = 0;

			std::string_view header_str((char*)buffer.data().data(), buffer.size());

			auto content_length_str = parse_key(header_str, "Content-Length");

			if (!content_length_str.empty())
			{
				content_length = std::atoll(content_length_str.data());
			}

			if (content_length > 0)
			{
				auto remain_size = static_cast<int64_t>(content_length - buffer.size());

				if (remain_size > 0)
				{
					ec = co_await session_ptr_->async_read(buffer, remain_size);
				}
			}
			else
			{
				ec = asio::error::eof;
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

		template <bool Server>
		auto parse_command_line(std::string_view header_span, error_code& ec)
		{
			using result_t =
				std::conditional_t<Server,
								   std::tuple<http_method, boost::system::result<boost::urls::url_view>, http_version>,
								   std::tuple<http_version, http_status>>;

			ec = boost::asio::error::eof;

			auto sp = header_span | std::views::split(' ');

			auto size = std::ranges::distance(sp.begin(), sp.end());

			if (size != header_part)
			{
				return result_t{};
			}

			auto iter = sp.begin();

			if constexpr (Server)
			{
				auto method = string_to_method(std::string_view(*iter++));

				if (!method.has_value())
				{
					return result_t{};
				}

				auto url_result = boost::urls::parse_origin_form(std::string_view(*iter++));
				if (url_result.has_error())
				{
					ec = url_result.error();
					return result_t{};
				}

				auto version = string_to_version(std::string_view(*iter));

				if (!version.has_value())
				{
					ec = version.error();
					return result_t{};
				}

				ec = error_code{};

				return std::make_tuple(*method, url_result, *version);
			}
			else
			{
				auto version = string_to_version(std::string_view(*iter++));

				if (!version.has_value())
				{
					ec = version.error();
					return result_t{};
				}

				auto status = string_to_status(std::string_view(*iter++));

				ec = error_code{};

				return std::make_tuple(*version, status);
			}
		}

		std::string_view parse_key(std::string_view header_str, const std::string& key)
		{
			std::string_view result{};

			auto pos = header_str.find(key);
			if (pos != std::string::npos)
			{
				auto end_pos = header_str.find_first_of("\r\n", pos);
				if (end_pos != std::string::npos)
				{
					auto start = header_str.find_first_of(':', pos);
					auto k = header_str.substr(pos, start - pos);
					if (k == key)
					{
						result = header_str.substr(start + 2, end_pos - start - 2);
					}
				}
			}

			return result;
		}

		auto make_error_response(error_code ec) -> asio::awaitable<void>
		{
			auto resp_header = std::format("{} {} {}", version_to_string(global_http_version), ec.value(),
										   status_to_string(ec.value()));
			flex_buffer error_buffer{};
			error_buffer.sputn(resp_header.c_str(), resp_header.size());
			co_await session_ptr_->async_send(std::move(error_buffer));
		}

		template <typename Handler, typename Func>
		auto handle_request(std::shared_ptr<Handler> handler_ptr, Func&& func, http_method method)
			-> asio::awaitable<error_code>
		{
			handler_ptr->request()->method(method);
			handler_ptr->request()->body().set_method(method);

			auto ec = co_await handler_ptr->handle();

			if (ec)
			{
				co_return ec;
			}

			flex_buffer resp_buffer{};
			handler_ptr->response().commit(resp_buffer);

			auto res = co_await func(resp_buffer, ec.value());

			if (res)
			{
				XLOG_ERROR() << "[http handle_request] async_send error:" << ec.what();
			}

			co_return res;
		}

		template <typename Request, typename Func>
		auto send_request(std::shared_ptr<Request> request, Func&& func, error_code& ec, http_method method, http_version version) -> asio::awaitable<std::size_t>
		{
			flex_buffer buffer{};

			std::string raw_header = std::format("{} {} {}", method_to_string(method), Request::this_router, version_to_string(version));

			buffer.sputn(raw_header.c_str(), raw_header.size());

			request->commit(buffer);

			regist_resp_func(func);

			ec = co_await session_ptr_->async_send(buffer);

			co_return 0;
		}

		template <typename Func>
		auto send_buffer(flex_buffer& req, const std::string& router, Func&& f, error_code& ec, http_method method, http_version version) -> asio::awaitable<std::size_t>
		{
			std::string raw_header = std::format("{} {} {}", method_to_string(method), router, version_to_string(version));

			regist_resp_func(std::forward<Func>(f));

			std::vector<asio::const_buffer> buffers{};

			commit_raw_header(buffers, raw_header);
			
			buffers.push_back(req.data());

			ec = co_await session_ptr_->async_send(buffers);

			co_return 0;
		}

		auto filling_buffer( flex_buffer& buffer) -> asio::awaitable<bool>
		{
			auto iter = buffers_.find(0);

			if (iter == buffers_.end())
			{
				buffer_channel_.try_send(error_code{}, 0);

				co_return false;
			}

			if (iter->second->func)
			{
				co_await iter->second->func(buffer, "");
			}

			iter->second->complete = true;

			buffers_.erase(iter);

			buffer_channel_.try_send(error_code{}, 0);

			co_return true;
		}

		template <typename Func>
		void regist_resp_func(Func&& func)
		{
			auto& cb = buffers_[0];
			if (!cb)
			{
				cb = std::make_shared<callback>();
			}

			cb->func = func;
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

		void commit_raw_header(std::vector<asio::const_buffer>& buffers, const std::string& header)
		{
			buffers.push_back(asio::const_buffer(header.c_str(), header.size()));
		}

	private:
		asio::experimental::channel<void(error_code, std::size_t)> buffer_channel_;

		std::map<std::size_t, std::shared_ptr<callback>> buffers_;

		std::shared_ptr<session_type> session_ptr_;
	};
} // namespace aquarius