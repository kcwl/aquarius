#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/http/http_header.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <virgo::http_method Method, typename Body, virgo::http_version Version = virgo::http_version::http1_1>
		class http_response : public basic_http_protocol<false, Method, Body>
		{
		public:
			using base = basic_http_protocol<false, Method, Body>;

			using base::has_request;

		public:
			http_response() = default;

			virtual ~http_response() = default;

			http_response(const http_response&) = delete;
			http_response& operator=(const http_response&) = delete;

			http_response(http_response&&) noexcept = default;
			http_response& operator=(http_response&&) noexcept = default;

		public:
			virtual error_code commit(flex_buffer& buffer) override
			{
				this->commit_command_header(buffer);

				flex_buffer buf{};

				this->body().serialize(buf);

				this->header().content_length(buf.size());

				auto ec = this->header().serialize(buffer);

				if (!ec)
				{
					if (this->header().content_length() != 0)
					{
						buffer.sputn((char*)buf.data().data(), buf.size());
					}
				}

				return ec;
			}

		protected:
			virtual void commit_command_header(flex_buffer& buffer) override
			{
				std::string headerline =
					std::format("{} {} {}\r\n", virgo::from_string_version(Version), static_cast<int>(this->result()),
								virgo::from_status_string(this->result()));

				buffer.sputn(headerline.c_str(), headerline.size());
			}

			virtual bool commit_cookie(flex_buffer& buffer) override
			{
				std::string line = "Set-Cookie: ";

				for (const auto& cookie : this->header().cookies())
				{
					line += cookie.first;

					if (!cookie.second.empty())
					{
						line += "=";
						line += cookie.second;
					}
					line += ";";
				}

				buffer.sputn(line.c_str(), line.size());

				return true;
			}

			virtual bool consume_cookie() override
			{
				auto cookie_value = this->header().find("Set-Cookie");

				auto cookie_pairs = std::span<char>(cookie_value) | std::views::split(";"sv);

				for (const auto& cookie : cookie_pairs)
				{
					auto str = std::string_view(cookie);

					if (str.empty())
						break;

					auto pos = str.find("=");

					if (pos == std::string_view::npos)
					{
						this->header().set_cookie(std::string(str));
					}
					else
					{
						auto key = str.substr(0, pos);
						auto value = str.substr(pos + 1, str.size() - pos - 1);

						this->header().set_cookie(std::string(key), std::string(value));
					}
				}

				return true;
			}
		};
	} // namespace virgo

	template <virgo::http_method Method, typename Body, virgo::http_version Version>
	struct is_message_type<virgo::http_response<Method, Body, Version>> : std::true_type
	{};
} // namespace aquarius