#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/module/channel_module.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/timer.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <boost/url.hpp>
#include <ranges>
#include <span>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace virgo
	{
		class http_null_response;
	}
} // namespace aquarius

namespace aquarius
{
	template <typename Protocol, template <typename> typename Adaptor, typename HandlerSelector>
	class session : public basic_session<Protocol, Adaptor>,
					public std::enable_shared_from_this<session<Protocol, Adaptor, HandlerSelector>>
	{
	public:
		using base_type = basic_session<Protocol, Adaptor>;

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

					uint32_t req{};

					ec = co_await recv(buffer, req);

					if (ec)
					{
						break;
					}

					auto router = binary_parse{}.from_datas<std::string>(buffer);

					XLOG_INFO() << "[accept] parse protocol router: " << router;

					HandlerSelector selector(req);

					selector(router, this->shared_from_this(), buffer);
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		auto query(std::size_t seq_number, error_code& ec) -> awaitable<flex_buffer>
		{
			flex_buffer buffer{};

			ec = error_code{};

			auto iter = buffer_controller_.find(seq_number);

			if (iter != buffer_controller_.end())
			{
				XLOG_INFO() << "[query from controll buffer] seq_number: " << seq_number;

				buffer = std::move(iter->second);

				buffer_controller_.erase(iter);
			}
			else
			{
				for (;;)
				{
					uint32_t seq{};
					ec = co_await recv(buffer, seq);
					if (ec)
					{
						break;
					}

					if (seq == seq_number)
					{
						co_return buffer;
					}

					buffer.pubseekoff(0 - sizeof(uint32_t), std::ios::cur);

					XLOG_INFO() << "[query controll buffer] seq_number: " << seq << ", wait seq: " << seq_number;

					buffer_controller_.emplace(seq, std::move(buffer));
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}

			co_return buffer;
		}

	private:
		auto recv(flex_buffer& buffer, uint32_t& req) -> awaitable<error_code>
		{
			uint32_t packet_length{};

			constexpr auto len = sizeof(packet_length);

			auto ec = co_await this->async_read(buffer, len);

			if (!ec)
			{
				buffer.sgetn((char*)&packet_length, sizeof(packet_length));

				ec = co_await this->async_read(buffer, packet_length);
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

			// router<Session>::get_mutable_instance().invoke(ping_router,
			// this->shared_from_this(), buffer);
		}

	private:
		timer<boost::asio::steady_timer> timer_;

		std::map<std::size_t, flex_buffer> buffer_controller_;
	};

	template <template <typename> typename Adaptor, typename HandlerSelector>
	class session<http_protocol, Adaptor, HandlerSelector>
		: public basic_session<http_protocol, Adaptor>,
		  public std::enable_shared_from_this<session<http_protocol, Adaptor, HandlerSelector>>
	{
		constexpr static auto two_crlf = "\r\n\r\n"sv;

		constexpr static auto crlf = "\r\n"sv;

		constexpr static std::size_t header_part = 3;

		using self_type = session<http_protocol, Adaptor, HandlerSelector>;

	public:
		using base_type = basic_session<http_protocol, Adaptor>;

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
				co_await server_recv(buffer, ec);
				if (ec)
				{
					break;
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		auto query(std::size_t seq_number, error_code& ec) -> awaitable<flex_buffer>
		{
			flex_buffer packet{};

			auto iter = buffer_controller_.find(seq_number);

			if (iter != buffer_controller_.end())
			{
				XLOG_INFO() << "[query from controll buffer] seq_number: " << seq_number;

				packet = std::move(iter->second);

				buffer_controller_.erase(iter);
			}
			else
			{
				for (;;)
				{
					auto size = packet.size();
					uint32_t seq{};
					co_await recv(packet, seq, ec);

					if (ec)
					{
						break;
					}

					if (seq != seq_number)
					{
						XLOG_INFO() << "[query controll buffer] seq_number: " << seq << ", wait seq: " << seq_number;

						packet.pubseekoff(size, std::ios::beg);

						buffer_controller_.emplace(seq, std::move(packet));
						continue;
					}

					break;
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "[query buffer] error: " << ec.what();
			}

			co_return std::move(packet);
		}

	private:
		auto server_recv(flex_buffer& buffer, error_code& ec) -> awaitable<void>
		{
			ec = co_await this->async_read_util(buffer, crlf);

			if (ec)
			{
				co_return;
			}

			auto header_line = std::string((char*)buffer.data().data(), buffer.size());

			auto [method, url, version] = parse_command_line<true>(std::span<char>(header_line), ec);

			std::string router(std::string_view(url->path().data(), url->path().size()));

			// auto content_type = hf.find("content-type");

			// if (!content_type.empty() && content_type != "application/json" && method == virgo::http_method::get)
			//{
			//	router = __http_source_handler__;
			// }
			// else if (method == virgo::http_method::get)
			//{
			//	if (!path.empty())
			//	{
			//		buffer.sputn(path.data(), path.size());
			//	}
			// }
			// else if (method == virgo::http_method::options)
			//{
			//	router = __http_options_handler__;
			// }

			auto handler_ptr = co_await mpc_publish<self_type>(router);

			auto request_ptr = handler_ptr->request();

			ec = co_await this->async_read_util(buffer, two_crlf);

			if (ec)
			{
				co_return;
			}

			request_ptr->consume_header(buffer);

			auto content_length = request_ptr->content_length();

			if (content_length != 0)
			{
				auto remain_size = static_cast<int64_t>(content_length - buffer.size());

				if (remain_size > 0)
				{
					ec = co_await this->async_read(buffer, remain_size);
				}
			}

			if (ec)
			{
				co_return;
			}

			request_ptr->consume_body(buffer);

			HandlerSelector()(handler_ptr, this->shared_from_this());
		}

		auto recv(flex_buffer& buffer, uint32_t& req, error_code& ec) -> awaitable<void>
		{
			ec = co_await this->async_read_util(buffer, crlf);

			if (ec)
			{
				co_return;
			}

			auto header_line = std::string((char*)buffer.data().data(), buffer.size());

			auto [version, status] = parse_command_line<false>(std::span<char>(header_line), ec);

			virgo::http_null_response resp{};

			resp.version(static_cast<int32_t>(version));
			resp.result(static_cast<int32_t>(status));

			ec = co_await this->async_read_util(buffer, two_crlf);

			if (ec)
			{
				co_return;
			}

			resp.consume_header(buffer);

			auto content_length = resp.content_length();

			if (content_length != 0)
			{
				auto remain_size = static_cast<int64_t>(content_length - buffer.size());

				if (remain_size > 0)
				{
					ec = co_await this->async_read(buffer, remain_size);
				}
			}

			req = resp.seq_number();
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

		std::map<std::size_t, flex_buffer> buffer_controller_;
	};

	template <typename Protocol, template <typename> typename Adaptor, typename HandlerSelector>
	struct is_session_type<session<Protocol, Adaptor, HandlerSelector>> : std::true_type
	{};
} // namespace aquarius