#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/timer.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <boost/url.hpp>
#include <ranges>
#include <span>

using namespace std::chrono_literals;

namespace aquarius
{
	template <auto Tag, template <bool, typename, typename> typename Adaptor, typename HandlerSelector>
	class session : public basic_session<Tag, Adaptor>,
					public std::enable_shared_from_this<session<Tag, Adaptor, HandlerSelector>>
	{
		struct proto_header
		{
			uint32_t length;
			uint32_t seq_number;
		};

	public:
		using base_type = basic_session<Tag, Adaptor>;

		using typename base_type::socket;

		using typename base_type::duration;

	public:
		session(socket socket_, duration timer, duration timeout)
			: base_type(std::move(socket_), timeout)
			, timer_(this->get_executor(), timer)
		{}

	public:
		auto accept() -> awaitable<error_code>
		{
			auto ec = co_await this->socket_adaptor_.accept(this->timeout_);

			if (!ec)
			{
				for (;;)
				{
					flex_buffer buffer{};
					virgo::header_fields hf;

					ec = co_await recv(buffer, hf);

					if (ec)
					{
						break;
					}

					auto router = binary_parse{}.from_datas<std::string>(buffer);

					XLOG_INFO() << "[accept] parse protocol router: " << router;

					HandlerSelector()(router, this->shared_from_this(), hf, buffer);
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Response>
		auto query(Response& resp, std::size_t seq_number, virgo::header_fields& hf, error_code& ec) -> awaitable<void>
		{
			for (;;)
			{
				flex_buffer buffer{};

				ec = co_await recv(buffer, hf);
				if (ec)
				{
					break;
				}

				auto iter = buffer_controller_.find(hf.seq_number());

				if (iter != buffer_controller_.end())
				{
					XLOG_INFO() << "[query from controll buffer] seq_number: " << hf.seq_number();

					buffer = std::move(iter->second);

					buffer_controller_.erase(iter);
				}
				else if (hf.seq_number() != seq_number)
				{
					XLOG_INFO() << "[query controll buffer] seq_number: " << hf.seq_number()
								<< ", wait seq: " << seq_number;

					buffer_controller_.emplace(hf.seq_number(), std::move(buffer));
					continue;
				}

				resp.set_header_fields(std::move(hf));
				resp.consume(buffer);
				break;
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}
		}

	private:
		auto recv(flex_buffer& buffer, virgo::header_fields& hf) -> awaitable<error_code>
		{
			proto_header proto{};

			constexpr auto len = sizeof(proto);

			auto ec = co_await this->async_read(buffer, len);

			if (!ec)
			{
				buffer.sgetn((char*)&proto, sizeof(proto_header));

				ec = co_await this->async_read(buffer, proto.length);

				hf.set_field("seq_number", std::to_string(proto.seq_number));
			}

			co_return ec;
		}

		void start_timer()
		{
			timer_.async_wait(
				[this, self = this->shared_from_this()](error_code ec)
				{
					if (ec)
					{
						XLOG_ERROR() << "timer error: " << ec.what();
						return;
					}

					do_timer();
				});
		}

		void do_timer()
		{
			flex_buffer buffer{};

			constexpr std::string_view ping_router = "tcp_ping"sv;

			// router<Session>::get_mutable_instance().invoke(ping_router, this->shared_from_this(), buffer);
		}

	private:
		timer<boost::asio::steady_timer> timer_;

		std::map<std::size_t, flex_buffer> buffer_controller_;
	};

	template <template <bool, typename, typename> typename Adaptor, typename HandlerSelector>
	class session<proto::http, Adaptor, HandlerSelector>
		: public basic_session<proto::http, Adaptor>,
		  public std::enable_shared_from_this<session<proto::http, Adaptor, HandlerSelector>>
	{
		constexpr static auto two_crlf = "\r\n\r\n"sv;

		constexpr static auto crlf = "\r\n"sv;

		constexpr static std::size_t header_part = 3;

		struct http_packet
		{
			virgo::http_version version;
			virgo::http_status status;
			virgo::header_fields hf;
			flex_buffer body;
		};

	public:
		using base_type = basic_session<proto::http, Adaptor>;

		using typename base_type::socket;

		using typename base_type::duration;

	public:
		session(socket socket_, duration timer, duration timeout)
			: base_type(std::move(socket_), timeout)
			, timer_(this->get_executor(), timer)
		{}

	public:
		auto accept() -> awaitable<error_code>
		{
			auto ec = co_await this->socket_adaptor_.accept(this->timeout_);

			for (;;)
			{
				flex_buffer buffer{};
				virgo::header_fields hf{};
				std::string header_command_line{};

				ec = co_await recv(buffer, header_command_line, hf);
				if (ec)
				{
					break;
				}

				auto [method, url, version] = parse_command_line<true>(std::span(header_command_line), ec);

				if (ec)
					break;

				HandlerSelector(method, version, std::string(url->encoded_params().buffer().data(), url->encoded_params().buffer().size()))(std::string_view(url->path().data(), url->path().size()),
												 this->shared_from_this(), hf, buffer);
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Response>
		auto query(Response& resp, std::size_t seq_number, virgo::header_fields& hf, error_code& ec) -> awaitable<void>
		{
			for (;;)
			{
				http_packet packet{};
				std::string header_command_line{};

				ec = co_await recv(packet.body, header_command_line, packet.hf);

				if (ec)
				{
					break;
				}

				auto iter = buffer_controller_.find(packet.hf.seq_number());

				if (iter != buffer_controller_.end())
				{
					XLOG_INFO() << "[query from controll buffer] seq_number: " << packet.hf.seq_number();

					packet = std::move(iter->second);

					buffer_controller_.erase(iter);
				}
				else if (packet.hf.seq_number() != seq_number)
				{
					XLOG_INFO() << "[query controll buffer] seq_number: " << packet.hf.seq_number()
								<< ", wait seq: " << seq_number;

					buffer_controller_.emplace(packet.hf.seq_number(), std::move(packet));
					continue;
				}

				resp.header().result(static_cast<int>(packet.status));
				resp.set_header_fields(packet.hf);
				resp.consume(packet.body);

				break;
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}
		}

	private:
		auto recv(flex_buffer& buffer, std::string& header_command_line, virgo::header_fields& hf)
			-> awaitable<error_code>
		{
			auto ec = co_await this->async_read_util(buffer, two_crlf);

			if (!ec)
			{
				auto proto_span_buffer = std::span<char>((char*)buffer.data().data(), buffer.data().size());

				auto proto_slide_buffer = proto_span_buffer | std::views::slide(two_crlf.size());

				auto iter =
					std::ranges::find_if(proto_slide_buffer, [&](auto c) { return std::string_view(c) == two_crlf; });

				auto len = std::ranges::distance(proto_slide_buffer.begin(), iter);

				auto header_span = proto_span_buffer.subspan(0, len);

				buffer.consume(len + two_crlf.size());

				header_command_line = parse_field(header_span, hf, ec);

				if (ec)
				{
					co_return ec;
				}

				auto content_length = hf.content_length();

				if (content_length != 0)
				{
					auto remain_size = static_cast<int64_t>(content_length - buffer.size());

					if (remain_size > 0)
					{
						ec = co_await this->async_read(buffer, remain_size);
					}
				}
			}

			co_return ec;
		}

		std::string parse_field(std::span<char> buffer, virgo::header_fields& hf, error_code& ec)
		{
			ec = error_code{};

			auto headers = buffer | std::views::split(crlf);

			auto len = std::ranges::distance(headers.begin(), headers.end());

			std::string header_command_line{};

			if (len == 0)
			{
				ec = boost::asio::error::eof;
				return header_command_line;
			}

			auto iter = headers.begin();

			header_command_line = std::string(std::string_view(*iter));

			while (++iter != headers.end())
			{
				auto str = std::string_view(*iter);
				auto pos = str.find(":");

				if (pos == std::string_view::npos)
				{
					ec = boost::asio::error::eof;

					break;
				}

				auto key = str.substr(0, pos);
				auto value = str.substr(pos + 1, str.size() - pos - 1);

				hf.set_field(std::string(key), std::string(value));
			}

			return header_command_line;
		}

		template <bool Server>
		auto parse_command_line(std::span<char> header_span, error_code& ec)
		{
			using result_t = std::conditional_t<
				Server,
				std::tuple<virgo::http_method, boost::system::result<boost::urls::url_view>, virgo::http_version>,
				std::tuple<virgo::http_version, virgo::http_status>>;

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
				auto method = virgo::from_string_method(std::string_view(*iter++));

				if (method == virgo::http_method::none)
				{
					return result_t{};
				}

				auto url_result = boost::urls::parse_origin_form(std::string_view(*iter++));
				if (url_result.has_error())
				{
					return result_t{};
				}

				auto version = virgo::from_version_string(std::string_view(*iter));

				if (version == virgo::http_version::unknown)
				{
					return result_t{};
				}

				ec = error_code{};

				return std::make_tuple(method, url_result, version);
			}
			else
			{
				auto version = virgo::from_version_string(std::string_view(*iter++));

				auto status = static_cast<virgo::http_status>(std::atoi(std::string_view(*iter++).data()));

				ec = error_code{};

				return std::make_tuple(version, status);
			}
		}

	private:
		timer<boost::asio::steady_timer> timer_;

		std::map<std::size_t, http_packet> buffer_controller_;
	};

	template <auto Tag, template <bool, typename, typename> typename Adaptor, typename HandlerSelector>
	struct is_session_type<session<Tag, Adaptor, HandlerSelector>> : std::true_type
	{};
} // namespace aquarius