#pragma once
#include <aquarius/serialize/binary.hpp>
#include <aquarius/ip/router.hpp>
#include <string_view>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/asio.hpp>
#include <aquarius/ip/http/http_header.hpp>
#include <aquarius/virgo/http_response.hpp>
#include <ranges>
#include <aquarius/virgo/http_get_body.hpp>
#include <aquarius/ip/http/http_param.hpp>
#include <aquarius/logger.hpp>
#include <fstream>

using namespace std::string_view_literals;

namespace aquarius
{
	template <auto Tag>
	struct processor
	{
		template <typename Session>
		auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<void>
		{
			error_code ec{};

			for (;;)
			{
				flex_buffer buffer{};

				uint32_t length = 0;

				constexpr auto len = sizeof(length);

				ec = co_await session_ptr->async_read(buffer, len);

				if (ec)
				{
					break;
				}

				buffer.sgetn((char*)&length, sizeof(uint32_t));

				ec = co_await session_ptr->async_read(buffer, length);

				if (ec)
				{
					break;
				}

				auto topic = binary_parse{}.from_datas<std::string>(buffer);

				router<Tag, Session>::get_mutable_instance().invoke(topic, session_ptr, buffer);
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << from_tag_string(Tag) << " read occur error - " << ec.message()
							 << ", remote_address: " << session_ptr->remote_address();
			}

			session_ptr->shutdown();
		}

		template <typename Response, typename Session>
		auto query(std::shared_ptr<Session> session_ptr) -> awaitable<Response>
		{
			flex_buffer buffer{};

			uint32_t length = 0;

			constexpr auto len = sizeof(length);

			auto ec = co_await session_ptr->async_read(buffer, len);

			if (ec)
			{
				XLOG_ERROR() << from_tag_string(Tag) << " query occur error - " << ec.message()
							 << ", remote_address: " << session_ptr->remote_address();
				co_return Response{};
			}

			buffer.sgetn((char*)&length, len);

			ec = co_await session_ptr->async_read(buffer, length);

			if (ec)
			{
				XLOG_ERROR() << from_tag_string(Tag) << " query occur error - " << ec.message()
							 << ", remote_address: " << session_ptr->remote_address();
				co_return Response{};
			}

			Response resp{};
			resp.consume(buffer);

			co_return resp;
		}

		template <typename Session>
		void do_timer(std::shared_ptr<Session> session_ptr)
		{
			flex_buffer buffer{};

			constexpr std::string_view ping_router = "tcp_ping"sv;

			router<Tag, Session>::get_mutable_instance().invoke(ping_router, session_ptr, buffer);
		}
	};

	template <>
	struct processor<protocol_tag::http>
	{
		constexpr static auto two_crlf = "\r\n\r\n"sv;

		constexpr static auto crlf = "\r\n"sv;

		auto parse_field(std::span<char> buffer, error_code& ec) -> virgo::http_fields
		{
			ec = virgo::http_status::ok;

			virgo::http_fields field;

			for (const auto& f : buffer | std::views::split(crlf))
			{
				if (std::string_view(f).empty())
					break;

				auto pos = std::string_view(f).find(":");
				if (pos == std::string_view::npos)
				{
					ec = virgo::http_status::bad_request;
					XLOG_ERROR() << "parse http header error[" << ec.value() << "]";

					return {};
				}

				auto key = std::string_view(f).substr(0, pos);
				auto value = std::string_view(f).substr(pos + 1, f.size() - pos - 1);

				field.set_field(key, value);
			}

			return field;
		}

		template <typename Session>
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

				buffer.consume(len + two_crlf.size());

				auto slide_proto_buffer = header_span | std::views::slide(crlf.size());

				auto it =
					std::ranges::find_if(slide_proto_buffer, [](const auto c) { return std::string_view(c) == crlf; });
				if (it == slide_proto_buffer.end())
					break;

				len = std::ranges::distance(slide_proto_buffer.begin(), it);

				error_code http_ec{};
				auto [method, _router, path, version] = parse_command_line<true>(proto_buffer.subspan(0, len), http_ec);

				if (http_ec.value() != static_cast<int>(virgo::http_status::ok))
				{
					co_return co_await make_error_response(session_ptr,
														   static_cast<virgo::http_status>(http_ec.value()));
				}

				auto hf = parse_field(proto_buffer.subspan(len + crlf.size()), http_ec);

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

						ec = co_await session_ptr->async_read(buffer, remain_size);

						if (ec)
							break;
					}
				}

				if (check_redirect(session_ptr, _router, hf, buffer))
				{
					continue;
				}

				if (check_get_search(method, path))
				{
					co_spawn(
						session_ptr->get_executor(),
						[&] -> awaitable<void>
						{
							using http_get_response = virgo::http_response<virgo::http_method::get,
																		   http_response_header, virgo::http_get_body>;

							http_get_response get_resp{};
							get_resp.version(virgo::http_version::http1_1);
							get_resp.set_field("Server", "Aqarius 1.00.0");
							get_resp.set_field("Connection", "keep-alive");
							get_resp.set_field("Access-Control-Allow-Origin", get_http_param().control_allow_origin);

							auto file_path = get_http_param().root_dir + std::string(_router.data(), _router.size());

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

							// co_await http_router<Session>::get_mutable_instance().send_response(session_ptr,
							// get_resp);
							flex_buffer buffer{};
							get_resp.commit(buffer);

							co_await session_ptr->async_send(buffer);
						},
						detached);

					continue;
				}

				if (method == virgo::http_method::get && !path.empty())
				{
					auto url = url_decode(std::string(path.data(), path.size()));

					buffer.sputn(path.data(), path.size());
				}

				router<protocol_tag::http, Session>::get_mutable_instance().invoke(_router, session_ptr, hf, buffer);
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << from_tag_string(protocol_tag::http) << " read occur error[" << ec.value() << "]"
							 << ", remote_address: " << session_ptr->remote_address();
			}

			session_ptr->shutdown();
		}

		template <typename Session>
		auto make_error_response(std::shared_ptr<Session> session_ptr, virgo::http_status status) -> awaitable<void>
		{
			std::string header{};
			auto make_command_line = [&]()
			{
				header += std::format("{} {} {}\r\n", virgo::from_string_version(get_http_param().version),
									  static_cast<int>(status), virgo::from_status_string(status));
			};

			make_command_line();

			flex_buffer buffer{};
			buffer.sputn(header.c_str(), header.size());

			co_await session_ptr->async_send(buffer);
		}
		template <bool Server>
		auto parse_command_line(std::span<char> header_span, error_code& ec) -> std::conditional_t<
			Server, std::tuple<virgo::http_method, std::string_view, std::string_view, virgo::http_version>,
			std::tuple<virgo::http_version, virgo::http_status>>
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

			if constexpr (Server)
			{
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

				return std::make_tuple(method, router, path, version);
			}
			else
			{
				auto version = virgo::from_version_string(std::string_view(*iter++));

				auto status = static_cast<virgo::http_status>(std::atoi(std::string_view(*iter++).data()));

				return std::make_tuple(version, status);
			}
		}

		template <typename Session>
		bool check_redirect(std::shared_ptr<Session> session_ptr, std::string_view router, virgo::http_fields hf,
							flex_buffer& buffer)
		{
			(void)session_ptr;
			(void)router;
			(void)hf;
			(void)buffer;
			// return http_router<Session>::get_mutable_instance().invoke(router == "/" ? "root" : router, session_ptr,
			// hf,
			//                                                          buffer);
			return false;
		}

		bool check_get_search(virgo::http_method method, std::string_view path)
		{
			return method == virgo::http_method::get && path.empty();
		}

		template <typename Session>
		auto check_get_search(std::shared_ptr<Session> session_ptr, std::string_view _router, std::string_view path,
							  virgo::http_method method, error_code& ec) -> awaitable<bool>
		{
			if (method != virgo::http_method::get || !path.empty())
				co_return false;

			using http_get_response =
				virgo::http_response<virgo::http_method::get, http_response_header, virgo::http_get_body>;

			http_get_response get_resp{};
			get_resp.version(virgo::http_version::http1_1);
			get_resp.set_field("Server", "Aqarius 1.00.0");
			get_resp.set_field("Connection", "keep-alive");
			get_resp.set_field("Access-Control-Allow-Origin", get_http_param().control_allow_origin);

			auto file_path = get_http_param().root_dir + std::string(_router.data(), _router.size());

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

			co_await router<protocol_tag::http, Session>::get_mutable_instance().send_response(session_ptr, get_resp);

			flex_buffer buffer{};
			get_resp.commit(buffer);

			co_await session_ptr->async_send(buffer);

			co_return true;
		}

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

			buffer.consume(len + two_crlf.size());

			auto slide_proto_buffer = header_span | std::views::slide(crlf.size());

			auto it =
				std::ranges::find_if(slide_proto_buffer, [](const auto c) { return std::string_view(c) == crlf; });
			if (it == slide_proto_buffer.end())
				co_return Response{};

			len = std::ranges::distance(slide_proto_buffer.begin(), it);

			error_code http_ec{};
			auto [version, status] = parse_command_line<false>(proto_buffer.subspan(0, len), http_ec);

			if (http_ec.value() != static_cast<int>(virgo::http_status::ok))
			{
				co_return Response{};
			}

			auto hf = parse_field(proto_buffer.subspan(len + crlf.size()), http_ec);

			if (http_ec.value() != static_cast<int>(virgo::http_status::ok))
			{
				co_return Response{};
			}

			Response resp{};
			resp.consume(buffer);

			co_return resp;
		}
	};
} // namespace aquarius