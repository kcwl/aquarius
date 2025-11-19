#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/http/http_router.hpp>
#include <ranges>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <aquarius/virgo/http_get_body.hpp>
#include <aquarius/ip/http/http_param.hpp>
#include <aquarius/virgo/http_response.hpp>
#include <aquarius/ip/http/http_header.hpp>
#include <fstream>
#include <aquarius/virgo/http_null_body.hpp>

namespace aquarius
{

	class http_base
	{
	public:
		using base = boost::asio::ip::tcp;

		constexpr static std::size_t max_http_length = 8192;

		constexpr static auto two_crlf = "\r\n\r\n"sv;

		constexpr static auto crlf = "\r\n"sv;

		using socket = base::socket;

		using endpoint = base::endpoint;

		using acceptor = base::acceptor;

		using resolver = base::resolver;

		using no_delay = base::no_delay;

	public:
		auto parse_field(std::span<char> buffer, error_code& ec) -> virgo::http_fields
		{
			ec = virgo::http_status::ok;

			virgo::http_fields field;

			for (const auto f : buffer | std::views::split(crlf))
			{
				auto pos = std::string_view(f).find(":");
				if (pos == std::string_view::npos)
				{
					ec = virgo::http_status::bad_request;
					return {};
				}

				auto key = std::string_view(f).substr(0, pos);
				auto value = std::string_view(f).substr(pos + 1, f.size() - pos - 1);

				field.set_field(key, value);
			}

			return field;
		}
	};

	template <bool Server>
	class http_proc : public http_base
	{
	public:
		template <typename Session>
		requires(is_session<Session>)
		auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<void>
		{
			error_code ec{};

			flex_buffer buffer{};

			for (;;)
			{
				ec = co_await session_ptr->async_read_util(buffer, two_crlf);

				if (ec)
					break;

				auto proto_buffer = std::span<char>((char*)buffer.data().data(), buffer.data().size());

				auto proto_slide_buffer = proto_buffer | std::views::slide(two_crlf.size());

				auto iter =
					std::ranges::find_if(proto_slide_buffer, [&](auto c) { return std::string_view(c) == two_crlf; });

				auto len = std::ranges::distance(proto_slide_buffer.begin(), iter);

				auto header_span = proto_buffer.subspan(0, len);

				auto it = std::ranges::find_if(header_span | std::views::slide(crlf.size()),
											   [&](auto c) { return std::string_view(c) == crlf; });

				if (it == proto_slide_buffer.end())
				{
					make_error_response(session_ptr, virgo::http_status::bad_request);
					continue;
				}

				auto header_len = std::ranges::distance(proto_slide_buffer.begin(), it);

				auto header_command = header_span.subspan(0, len);

				auto header_fields = header_span.subspan(header_len + crlf.size(), len - header_len - crlf.size());

				virgo::http_fields hf{};

				error_code http_ec{};
				auto [method, router, path, version] = parse_command_line(header_command, http_ec);

				if (http_ec.value() != static_cast<int>(virgo::http_status::ok))
				{
					co_return co_await make_error_response(session_ptr,
														   static_cast<virgo::http_status>(http_ec.value()));
				}

				hf = parse_field(header_fields, http_ec);

				if (http_ec.value() != static_cast<int>(virgo::http_status::ok))
				{
					co_return co_await make_error_response(session_ptr,
														   static_cast<virgo::http_status>(http_ec.value()));
				}

				// if (hf.content_length() == 0 && method_ != virgo::http_method::get)
				// {
				//     co_return co_await make_error_response(session_ptr, version_,hf,
				//     virgo::http_status::length_required);
				// }

				// if (hf.content_length() > max_http_length)
				// {
				//     co_return co_await make_error_response(session_ptr, version_,hf,
				//     virgo::http_status::payload_too_large);
				// }

				if (!hf.find("Expect").empty())
				{
					co_return co_await make_error_response(session_ptr, virgo::http_status::expectation_failed);
				}

				if (hf.content_length() != 0)
				{
					auto remain_size = static_cast<int64_t>(hf.content_length() - buffer.size());

					if (remain_size > 0)
					{
						ec = aquarius::error_code{};

						buffer.prepare(remain_size);

						ec = co_await session_ptr->async_read(buffer);

						if (ec)
							break;
					}
				}

				if (check_redirect(session_ptr, router, hf, buffer))
				{
					continue;
				}

				if (check_get_search(method, path))
				{
					co_spawn(
						session_ptr->get_executor(),
						[&] -> awaitable<void>
						{
							using http_get_response = virgo::http_response<"get", virgo::http_method::get,
																		   http_response_header, virgo::http_get_body>;

							http_get_response get_resp{};
							get_resp.version(virgo::http_version::http1_1);
							get_resp.set_field("Server", "Aqarius 1.00.0");
							get_resp.set_field("Connection", "keep-alive");
							get_resp.set_field("Access-Control-Allow-Origin", get_http_param().control_allow_origin);

							auto file_path = get_http_param().root_dir + std::string(router.data(), router.size());

							std::fstream ifs(file_path, std::ios::in | std::ios::binary);
							if (!ifs.is_open())
							{
								ec = virgo::http_status::not_found;
							}
							else
							{
								auto file_size = std::filesystem::file_size(file_path);

								get_resp.set_field("Content-Length", std::to_string(file_size));

								auto ext = std::filesystem::path(file_path).extension();

								std::string content_type = "text/html";

								if (ext == ".js")
								{
									content_type = "application/javascript";
								}
								else if (ext == ".css")
								{
									content_type = "text/css";
								}
								else if (ext == ".ico")
								{
									content_type = "image/*";
								}

								// content_type += "; charset=utf-8";

								get_resp.set_field("Content-Type", content_type);

								ec = virgo::http_status::ok;

								auto& stream = get_resp.body().stream_;
								stream.resize(file_size);
								ifs.read(stream.data(), stream.size());
							}

							//co_await http_router<Session>::get_mutable_instance().send_response(session_ptr, get_resp);
							co_return;
						},
						detached);

					continue;
				}

				if (method == virgo::http_method::get && !path.empty())
				{
					buffer.sputn(path.data(), path.size());
				}

				http_router<Session>::get_mutable_instance().invoke(router, session_ptr, hf, buffer);
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "on read some occur error - " << ec.message();
			}

			session_ptr->shutdown();
		}

	private:
		template <typename Session>
		auto make_error_response(std::shared_ptr<Session> session_ptr, virgo::http_status status) -> awaitable<void>
		{
			//using http_error_response = virgo::http_response<http_response_header, virgo::http_null_body>;

			//http_error_response resp{};
			//resp.result(status);

			//co_await http_router<Session>::get_mutable_instance().send_response(session_ptr, resp);
			co_return;
		}
		auto parse_command_line(std::span<char> header_span, error_code& ec)
			-> std::tuple<virgo::http_method, std::string_view, std::string_view, virgo::http_version>
		{
			ec = virgo::http_status::ok;

			auto sp = header_span | std::views::split(' ');

			auto size = std::ranges::distance(sp.begin(),sp.end());

			if (size != 3)
			{
				ec = virgo::http_status::bad_request;
				return {};
			}

			auto iter = sp.begin();

			auto method = virgo::from_string_method(std::string_view(*iter++));

			if (method == virgo::http_method::none)
				return {};

			auto uri = std::string_view(*iter++);

			auto uri_sp = uri | std::views::split('?');

			auto uri_iter = uri_sp.begin();

			auto router = std::string_view(*uri_iter);

			size = std::ranges::distance(uri_sp.begin(), uri_sp.end());

			std::string_view path{};

			if (size != 1)
			{
				std::string_view(*++uri_iter).swap(path);
			}

			if (router.empty())
			{
				ec = virgo::http_status::bad_request;
				return {};
			}

			auto version = virgo::from_version_string(std::string_view(*iter));

			if (version == virgo::http_version::unknown)
			{
				ec = virgo::http_status::bad_request;
				return {};
			}

			return { method, router, path, version };
		}

		template <typename Session>
		bool check_redirect(std::shared_ptr<Session> session_ptr, std::string_view router, virgo::http_fields hf,
							flex_buffer& buffer)
		{
			return http_router<Session>::get_mutable_instance().invoke(router == "/" ? "root" : router, session_ptr, hf, buffer);
		}

		bool check_get_search(virgo::http_method method, std::string_view path)
		{
			return method == virgo::http_method::get && path.empty();
		}

		template <typename Session>
		auto check_get_search(std::shared_ptr<Session> session_ptr, std::string_view router, std::string_view path,
							  virgo::http_method method, error_code& ec) -> awaitable<bool>
		{
			if (method != virgo::http_method::get || !path.empty())
				co_return false;

			using http_get_response = virgo::http_response<"get", http_response_header, virgo::http_get_body>;

			http_get_response get_resp{};
			get_resp.version(virgo::http_version::http1_1);
			get_resp.set_field("Server", "Aqarius 1.00.0");
			get_resp.set_field("Connection", "keep-alive");
			get_resp.set_field("Access-Control-Allow-Origin", get_http_param().control_allow_origin);

			auto file_path = get_http_param().root_dir + std::string(router.data(), router.size());

			std::fstream ifs(file_path, std::ios::in | std::ios::binary);
			if (!ifs.is_open())
			{
				ec = virgo::http_status::not_found;
			}
			else
			{
				auto file_size = std::filesystem::file_size(file_path);

				get_resp.set_field("Content-Length", std::to_string(file_size));

				auto ext = std::filesystem::path(file_path).extension();

				std::string content_type = "text/html";

				if (ext == ".js")
				{
					content_type = "application/javascript";
				}
				else if (ext == ".css")
				{
					content_type = "text/css";
				}
				else if (ext == ".ico")
				{
					content_type = "image/*";
				}

				// content_type += "; charset=utf-8";

				get_resp.set_field("Content-Type", content_type);

				ec = virgo::http_status::ok;

				auto& stream = get_resp.body().stream_;
				stream.resize(file_size);
				ifs.read(stream.data(), stream.size());
			}

			co_await http_router<Session>::get_mutable_instance().send_response(session_ptr, get_resp);

			co_return true;
		}
	};

	template <>
	class http_proc<false> : public http_base
	{
	public:
		template <typename Response, typename Session>
		auto query(std::shared_ptr<Session> session_ptr) -> awaitable<Response>
		{
			error_code ec{};

			flex_buffer buffer{};

			ec = co_await session_ptr->async_read_util(buffer, two_crlf);

			if (ec)
				co_return Response{};

			auto proto_buffer = std::span<char>((char*)buffer.data().data(), buffer.data().size());

			auto proto_slide_buffer = proto_buffer | std::views::slide(two_crlf.size());

			auto iter =
				std::ranges::find_if(proto_slide_buffer, [&](auto c) { return std::string_view(c) == two_crlf; });

			auto len = std::ranges::distance(proto_slide_buffer.begin(), iter);

			auto header_span = proto_buffer.subspan(0, len);

			auto it = std::ranges::find_if(header_span | std::views::slide(crlf.size()),
										   [&](auto c) { return std::string_view(c) == crlf; });

			if (it == proto_slide_buffer.end())
			{
				co_return Response{};
			}

			auto header_len = std::ranges::distance(proto_slide_buffer.begin(), it);

			auto header_command = header_span.subspan(0, len);

			auto header_fields = header_span.subspan(header_len + crlf.size(), len - header_len - crlf.size());

			auto [version, http_status] = parse_response_line(header_command, ec);

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
			{
				co_return Response{};
			}

			auto hf = parse_field(header_fields, ec);

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
			{
				co_return Response{};
			}

			Response resp{};

			resp.move_copy(std::move(hf));
			resp.consume(buffer);

			co_return resp;
		}

	private:
		auto parse_response_line(std::span<char> header_span, error_code& ec)
			-> std::tuple<virgo::http_version, virgo::http_status>
		{
			ec = virgo::http_status::ok;

			auto sp = header_span | std::views::split(' ');

			auto size = std::ranges::distance(sp.begin(), sp.end());

			if (size != 3)
			{
				ec = virgo::http_status::bad_request;
				return {};
			}

			auto iter = sp.begin();

			auto version = virgo::from_version_string(std::string_view(*iter++));

			auto status = static_cast<virgo::http_status>(std::atoi(std::string_view(*iter++).data()));

			return { version, status };
		}
	};
} // namespace aquarius
