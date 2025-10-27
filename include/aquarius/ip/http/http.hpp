#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/http/http_router.hpp>
#include <ranges>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{

	class http
	{
		using base = boost::asio::ip::tcp;

		constexpr static std::size_t max_http_length = 8192;

	public:
		using socket = base::socket;

		using endpoint = base::endpoint;

		using acceptor = base::acceptor;

		using resolver = base::resolver;

		using no_delay = base::no_delay;

	public:
		template <typename Session>
		requires(is_session<Session>)
		auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			error_code ec{};

			flex_buffer buffer{};

			for (;;)
			{
				co_await recv(session_ptr, buffer, ec);

				if (ec.value() != static_cast<int>(virgo::http_status::ok))
					break;
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "on read some occur error - " << ec.what();
			}

			session_ptr->shutdown();

			co_return ec;
		}

		template <typename Response, typename Session>
		auto query(std::shared_ptr<Session> session_ptr) -> awaitable<Response>
		{
			error_code ec{};

			flex_buffer buffer{};

			virgo::http_fields hf;

			co_await recv_buffer(session_ptr, buffer, hf, ec, false);

			Response resp{};

			if (ec.value() == static_cast<int>(virgo::http_status::ok))
			{
				resp.move_copy(std::move(hf));
				resp.consume(buffer);
			}

			co_return resp;
		}

		template <typename Request>
		void make_request_buffer(std::shared_ptr<Request> request, flex_buffer& buffer)
		{
			std::string str = std::format("{} {} {}\r\n", from_method_string(Request::method), Request::router,
										  from_version_string(request->version()));

			request->set_field("Content-Type", "aquarius");
			request->set_field("Server", "Aquarius 0.10.0");
			request->set_field("Connection", "keep-alive");
			request->set_field("Access-Control-Allow-Origin", "*");
			request->set_field("Content-Type", "aquarius-json");

			for (auto& s : request->fields())
			{
				str += std::format("{}: {}\r\n", s.first, s.second);
			}

			str += "\r\n";

			buffer.put(str.begin(), str.end());

			request->commit(buffer);
		}

	protected:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, flex_buffer& buffer, error_code& ec) -> awaitable<void>
		{
			virgo::http_fields hf;

			co_await recv_buffer(session_ptr, buffer, hf, ec);

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session_ptr->shutdown();
				co_return;
			}

			if (method_ == virgo::http_method::get)
			{
				flex_buffer((uint8_t*)this->get_body_span_.data(), this->get_body_span_.size()).swap(buffer);
			}

			co_spawn(
				session_ptr->get_executor(),
				[session_ptr, buffer = std::move(buffer), this, hf = std::move(hf)]() mutable -> awaitable<void>
				{
					http_router<Session>::get_mutable_instance().invoke(path_, session_ptr, hf, std::move(buffer));

					co_return;
				},
				detached);
		}

	private:
		template <typename Session>
		auto recv_buffer(std::shared_ptr<Session> session_ptr, flex_buffer& buffer, virgo::http_fields& hf,
						 error_code& ec, bool request = true) -> awaitable<void>
		{
			constexpr auto two_crlf = "\r\n\r\n"sv;

			ec = co_await session_ptr->async_read_util(buffer, two_crlf);

			if (ec)
				co_return;

			request ? parse_request_header_line(buffer, ec) : parse_response_header_line(buffer, ec);

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				co_return;

			auto span = std::span<char>((char*)buffer.rdata(), buffer.active());

			auto buf_view = span | std::views::slide(4);

			auto iter = std::ranges::find_if(buf_view,
											 [](const auto& value)
											 {
												 if (value.size() < 4)
													 return false;

												 return std::string_view(value) == two_crlf;
											 });

			if (iter == buf_view.end())
			{
				ec = make_error_code(virgo::http_status::bad_request);
				co_return;
			}

			auto len = std::distance(buf_view.begin(), iter);

			auto head_buf = std::span<char>((char*)buffer.rdata(), len);

			buffer.consume(len + 4);

			auto header_view = std::views::split(head_buf, '\n');

			for (const auto header : header_view)
			{
				auto itor =
					std::ranges::find_if(header.begin(), header.end(), [](const auto& value) { return value == ':'; });

				if (itor == header.end())
				{
					ec = virgo::http_status::bad_request;
					co_return;
				}

				len = std::distance(header.begin(), itor);

				auto key = std::string(std::string_view(header).substr(0, len));

				auto value = std::string(std::string_view(header).substr(len + 1));

				if (value.back() == '\r')
					value = value.substr(0, value.size() - 1);

				if (value.front() == ' ')
					value = value.substr(1);

				hf.set_field(key, value);
			}

			auto remain_size = static_cast<int64_t>(hf.content_length() - buffer.active());

			ec = virgo::http_status::ok;

			if (remain_size > 0)
				ec = co_await session_ptr->async_read(buffer, remain_size);
		}

		void parse_request_header_line(flex_buffer& buffer, error_code& ec)
		{
			auto span = std::span<char>((char*)buffer.rdata(), buffer.active());

			auto buf_view = span | std::views::slide(2);

			auto iter = std::ranges::find_if(buf_view,
											 [](const auto& v)
											 {
												 if (v.size() < 2)
													 return false;

												 return std::string_view(v) == "\r\n";
											 });

			if (iter == buf_view.end())
			{
				ec = virgo::http_status::bad_request;
				return;
			}

			auto len = std::distance(buf_view.begin(), iter);

			buffer.consume(len + 2);

			auto header_line = span.subspan(0, len + 2);

			auto headers = header_line | std::views::split(' ');

			auto size = std::distance(headers.begin(), headers.end());

			if (size < 3)
			{
				ec = virgo::http_status::bad_request;
				return;
			}

			auto header_iter = headers.begin();

			parse_method(std::span(*header_iter++), method_, ec);
			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return;

			parse_uri(std::span<char>(*header_iter++), ec);
			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return;

			parse_version(std::span(*header_iter), version_, ec);
		}

		void parse_response_header_line(flex_buffer& buffer, error_code& ec)
		{
			auto span = std::span<char>((char*)buffer.rdata(), buffer.active());

			auto buf_view = span | std::views::slide(2);

			auto iter = std::ranges::find_if(buf_view,
											 [](const auto& v)
											 {
												 if (v.size() < 2)
													 return false;

												 return std::string_view(v) == "\r\n";
											 });

			if (iter == buf_view.end())
			{
				ec = virgo::http_status::bad_request;
				return;
			}

			auto len = std::distance(buf_view.begin(), iter);

			// auto header_line = span.subspan(0, len + 2);
			auto header_line = std::span<char>((char*)buffer.rdata(), len + 2);

			buffer.consume(len + 2);

			auto headers = header_line | std::views::split(' ');

			auto size = std::distance(headers.begin(), headers.end());

			if (size < 3)
			{
				ec = virgo::http_status::bad_request;
				return;
			}

			auto header_iter = headers.begin();

			parse_version(std::span<char>(*header_iter++), version_, ec);

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return;

			status_ = static_cast<virgo::http_status>(std::atoi(std::string_view(*header_iter++).data()));

			auto reason_string_view = std::string_view(*header_iter);

			std::size_t view_size = reason_string_view.size();

			view_size >= 2 ? reason_string_view.size() - 2 : 0;

			reason_ = std::string(reason_string_view.data(), view_size);

			ec = virgo::http_status::ok;
		}

		template <typename T>
		void parse_method(std::span<T> buffer, virgo::http_method& method_, error_code& ec)
		{
			ec = virgo::http_status::ok;

			std::string_view value(buffer.data(), buffer.size());

			if (value == "POST")
			{
				method_ = virgo::http_method::post;
			}
			else if (value == "GET")
			{
				method_ = virgo::http_method::get;
			}
			else if (value == "OPTIONS")
			{
				method_ = virgo::http_method::options;
			}
			else
			{
				ec = virgo::http_status::bad_request;
			}
		}

		template <typename T>
		void parse_uri(std::span<T> buffer, error_code& ec)
		{
			ec = virgo::http_status::ok;

			if (buffer.empty())
			{
				ec = virgo::http_status::bad_request;
				return;
			}

			if (method_ != virgo::http_method::get)
			{
				path_ = std::string(buffer.data(), buffer.size());
				return;
			}

			auto iter = std::find(buffer.begin(), buffer.end(), '?');

			if (iter == buffer.end())
			{
				virgo::http_status::bad_request;
				return;
			}

			auto pos = std::distance(buffer.begin(), iter);

			auto body_span = buffer.subspan(pos + 1);

			get_body_span_ = std::string(body_span.data(), body_span.size());
		}

		template <typename T>
		void parse_version(std::span<T> buffer, virgo::http_version& version_, error_code& ec)
		{
			ec = make_error_code(virgo::http_status::ok);

			auto value = std::string_view(buffer.data());

			auto pos = value.find('.');
			if (pos == std::string::npos)
			{
				if (value == "HTTP2")
				{
					version_ = virgo::http_version::http2;
				}
				else if (value == "HTTP3")
				{
					version_ = virgo::http_version::http3;
				}
				else
				{
					ec = make_error_code(virgo::http_status::bad_request);
				}
			}
			else
			{
				auto suffix = value.substr(pos + 1);
				if (*suffix.data() == '1')
				{
					version_ = virgo::http_version::http1_1;
				}
				else if (*suffix.data() == '0')
				{
					version_ = virgo::http_version::http1_0;
				}
				else
				{
					ec = make_error_code(virgo::http_status::bad_request);
				}
			}
		}

		template <typename T>
		void parse_querys(std::span<T> buffer, std::vector<std::pair<std::string, std::string>>& querys_,
						  error_code& ec)
		{
			ec = make_error_code(virgo::http_status::ok);

			for (const auto& p : buffer | std::views::split('&'))
			{
				auto pos = std::string_view(p).find('=');
				if (pos == std::string_view::npos)
				{
					ec = make_error_code(virgo::http_status::bad_request);
					break;
				}

				querys_.emplace_back();
				auto& query = querys_.back();

				query.first = std::string(std::string_view(p).substr(0, pos));
				query.second = std::string(std::string_view(p).substr(pos + 1));
			}
		}

	private:
		virgo::http_method method_;

		virgo::http_version version_;

		std::string path_;

		virgo::http_status status_;

		std::string reason_;

		std::string get_body_span_;
	};
} // namespace aquarius
