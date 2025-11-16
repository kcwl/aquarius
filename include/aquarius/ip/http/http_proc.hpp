#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/http/http_router.hpp>
#include <ranges>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <aquarius/ip/make_protocol.hpp>
#include <aquarius/virgo/http_fields.hpp>
#include <aquarius/virgo/http_get_body.hpp>
#include <aquarius/ip/http/http_param.hpp>
#include <aquarius/virgo/http_response.hpp>
#include <aquarius/ip/http/http_header.hpp>
#include <fstream>

namespace aquarius
{
	class http_base
	{
	public:
		using base = boost::asio::ip::tcp;

		constexpr static std::size_t max_http_length = 8192;

		constexpr static auto two_crlf = "\r\n\r\n"sv;

		using socket = base::socket;

		using endpoint = base::endpoint;

		using acceptor = base::acceptor;

		using resolver = base::resolver;

		using no_delay = base::no_delay;

	public:
		auto parse_field(flex_buffer& buffer, error_code& ec) -> virgo::http_fields
		{
			ec = virgo::http_status::ok;

			virgo::http_fields field;

			char* data_pos = (char*)buffer.data().data();

			auto read_key = [&] -> std::string
			{
				data_pos = (char*)buffer.data().data();

				while (buffer.size() != 0)
				{
					auto_commit ac(buffer);

					if (check_end(buffer))
						return std::string((char*)data_pos, (char*)buffer.data().data() - (char*)data_pos);

					auto c = buffer.sgetc();

					if (c == flex_buffer::traits_type::eof())
					{
						break;
					}

					if (std::isspace(c))
					{
						break;
					}

					if (c == ':')
					{
						return std::string((char*)data_pos, (char*)buffer.data().data() - (char*)data_pos);
					}
				}

				ec = virgo::http_status::bad_request;

				return {};
			};

			auto read_value = [&] -> std::string
			{
				data_pos = (char*)buffer.data().data();

				while (buffer.size() != 0)
				{
					auto_consume ac(buffer);

					auto c = buffer.sgetc();

					if (c == flex_buffer::traits_type::eof())
						return {};

					if (check_end(buffer))
					{
						return std::string((char*)data_pos, (char*)buffer.data().data() - (char*)data_pos);
					}
				}

				ec = virgo::http_status::bad_request;
				return {};
			};

			while (buffer.size() != 0)
			{
				auto key = read_key();
				if (ec.value() != static_cast<int>(virgo::http_status::ok))
				{
					return {};
				}

				if (key == "\r")
				{
					auto_consume ac(buffer);
					break;
				}

				auto value = read_value();
				if (ec.value() != static_cast<int>(virgo::http_status::ok))
				{
					return {};
				}

				value = value.substr(0, value.size() - 1);

				field.set_field(key, value);
			}

			return field;
		}

	protected:
		bool check_end(flex_buffer& buffer)
		{
			bool result = false;

			auto c = buffer.sgetc();

			if (c != '\r')
				return result;

			buffer.consume(1);

			c = buffer.sgetc();

			if (c == '\n')
			{
				result = true;
			}
			else
			{
				buffer.pubseekoff(-1, std::ios::cur, std::ios::cur);
			}

			return result;
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

				virgo::http_fields hf{};

				error_code http_ec{};
				auto [method, router, path, version] = parse_request_line(buffer, http_ec);

				if (http_ec.value() != static_cast<int>(virgo::http_status::ok))
				{
					co_return co_await make_error_response(session_ptr, version, hf,
														   static_cast<virgo::http_status>(http_ec.value()));
				}

				hf = parse_field(buffer, http_ec);

				if (http_ec.value() != static_cast<int>(virgo::http_status::ok))
				{
					co_return co_await make_error_response(session_ptr, version, hf,
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
					co_return co_await make_error_response(session_ptr, version, hf,
														   virgo::http_status::expectation_failed);
				}

				if (hf.content_length() != 0)
				{
					auto remain_size = static_cast<int64_t>(hf.content_length() - buffer.size());

					if (remain_size > 0)
					{
						ec = aquarius::error_code{};

						buffer.prepare(remain_size);

						ec = co_await session_ptr->async_read(buffer);
					}
				}

				if (check_redirect(session_ptr, router, hf, buffer))
				{
					continue;
				}

				if (!co_await check_get_search(session_ptr, router == "/" ? "/index.html" : router, path, method, ec))
				{
					if (method == virgo::http_method::get && !path.empty())
					{
						buffer.sputn(path.data(), path.size());
					}

					co_spawn(
						session_ptr->get_executor(),
						[&, this, hf = std::move(hf)]() mutable -> awaitable<void>
						{
							http_router<Session>::get_mutable_instance().invoke(method, router, session_ptr, hf,
																				buffer);

							co_return;
						},
						detached);
				}
			}

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "on read some occur error - " << ec.message();
			}

			session_ptr->shutdown();
		}

	private:
		template <typename Session>
		auto make_error_response(std::shared_ptr<Session> session_ptr, virgo::http_version version,
								 const virgo::http_fields& hf, virgo::http_status status) -> awaitable<void>
		{
			flex_buffer buffer{};

			auto headline = make_http_headline(virgo::from_string_version(version), static_cast<int>(status),
											   virgo::from_status_string(status));

			for (const auto& s : hf.fields())
			{
				headline += std::format("{}: {}\r\n", s.first, s.second);
			}

			headline += "\r\n";

			buffer.sputn(headline.c_str(), headline.size());

			co_await session_ptr->async_send(std::move(buffer));
		}
		auto parse_request_line(flex_buffer& buffer, error_code& ec)
			-> std::tuple<virgo::http_method, std::string_view, std::string_view, virgo::http_version>
		{
			ec = virgo::http_status::ok;

			auto read_method = [&] -> virgo::http_method
			{
				auto data_pos = buffer.data().data();
				while (buffer.size() != 0)
				{
					auto_consume ac(buffer);
					auto c = buffer.sgetc();
					if (std::isspace(c))
						return virgo::from_string_method(
							std::string_view((char*)data_pos, (char*)buffer.data().data() - (char*)data_pos));
					if (!std::isalpha(c))
					{
						break;
					}
				}

				ec = virgo::http_status::bad_request;

				return {};
			};

			auto method = read_method();

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return {};

			auto read_uri = [&] -> std::tuple<std::string_view, std::string_view>
			{
				std::string_view router;
				std::string_view path;
				char* data_pos = (char*)buffer.data().data();
				while (buffer.size() != 0)
				{
					auto_consume ac(buffer);

					auto c = buffer.sgetc();

					if (c == ' ')
					{
						if (router.empty())
						{
							router = std::string_view(data_pos, (char*)buffer.data().data() - data_pos);

							return { router, path };
						}
						else
						{
							path = std::string_view(data_pos, (char*)buffer.data().data() - data_pos);
							return { router, path };
						}
					}

					if (c == '?')
					{
						path = std::string_view(data_pos, (char*)buffer.data().data() - data_pos);
						data_pos = (char*)buffer.data().data();
					}
				}

				ec = virgo::http_status::bad_request;

				return {};
			};

			auto [router, path] = read_uri();

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return {};

			auto read_version = [&] -> virgo::http_version
			{
				char* data_pos = (char*)buffer.data().data();
				while (buffer.size() != 0)
				{
					auto_consume ac(buffer);
					auto c = buffer.sgetc();

					if (check_end(buffer))
					{
						return virgo::from_version_string(
							std::string_view(data_pos, (char*)buffer.data().data() - data_pos));
					}

					if (!std::isalnum(c) && c != '/' && c != '.')
					{
						ec = virgo::http_status::http_version_not_supported;
						break;
					}
				}

				return {};
			};

			auto version = read_version();

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return {};

			return { method, router, path, version };
		}

		template <typename Session>
		bool check_redirect(std::shared_ptr<Session> session_ptr, std::string_view router, virgo::http_fields hf,
							flex_buffer& buffer)
		{
			return http_router<Session>::get_mutable_instance().invoke(virgo::http_method::redirect, router == "/" ? "root" : router, session_ptr, hf,
																	   buffer);
		}

		template<typename Session>
		auto check_get_search(std::shared_ptr<Session> session_ptr, std::string_view router, std::string_view path , virgo::http_method method, error_code& ec) -> awaitable<bool>
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

				//content_type += "; charset=utf-8";

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

			auto [version, http_status] = parse_response_line(buffer, ec);

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
			{
				co_return Response{};
			}

			auto hf = parse_field(buffer, ec);

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
		auto parse_response_line(flex_buffer& buffer, error_code& ec)
			-> std::tuple<virgo::http_version, virgo::http_status>
		{
			ec = virgo::http_status::ok;

			auto parse_version = [&] { return virgo::http_version{}; };

			auto version = parse_version();

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return {};

			auto to_status = [&] -> virgo::http_status
			{
				auto data_pos = buffer.data().data();
				int bit = 3;

				while (bit--)
				{
					auto_consume ac(buffer);

					auto c = buffer.sgetc();
					if (!std::isdigit(c))
					{
						ec = virgo::http_status::bad_request;
						return {};
					}
				}

				{
					auto_consume ac(buffer);

					auto c = buffer.sgetc();

					if (!std::isspace(c))
					{
						ec = virgo::http_status::bad_request;
						return {};
					}
				}

				auto status =
					std::stoi(std::string_view((char*)data_pos, (char*)buffer.data().data() - (char*)data_pos).data());

				return static_cast<virgo::http_status>(status);
			};

			auto status = to_status();

			if (ec.value() != static_cast<int>(virgo::http_status::ok))
				return {};

			while (buffer.size() != 0)
			{
				auto_consume ac(buffer);

				if (check_end(buffer))
				{
					return { version, status };
				}
			}

			ec = virgo::http_status::bad_request;

			return {};
		}
	};
} // namespace aquarius
