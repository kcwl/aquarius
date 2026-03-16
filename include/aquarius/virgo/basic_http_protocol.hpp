#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <ranges>
#include <span>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, virgo::http_method Method, typename Body, typename Allocator = std::allocator<Body>>
		class basic_http_protocol : public basic_tcp_protocol<Request, http_header, Body, Allocator>
		{
		public:
			using base = basic_tcp_protocol<Request, http_header, Body, Allocator>;

			using base::has_request;

		public:
			basic_http_protocol() = default;

			virtual ~basic_http_protocol() = default;

			basic_http_protocol(const basic_http_protocol&) = delete;

			basic_http_protocol& operator=(const basic_http_protocol&) = delete;

			basic_http_protocol(basic_http_protocol&&) noexcept = default;

			basic_http_protocol& operator=(basic_http_protocol&&) noexcept = default;

		public:
			virtual error_code commit(flex_buffer& buffer) override
			{
				this->commit_command_header(buffer);

				flex_buffer buf{};

				if constexpr (Method != virgo::http_method::get)
				{
					this->body().serialize(buf);

					this->header().content_length(buf.size());
				}

				commit_cookie(buffer);

				auto ec = this->header().serialize(buffer);

				if (!ec)
				{
					if constexpr (Method != virgo::http_method::get)
					{
						if (this->header().content_length() != 0)
						{
							buffer.sputn((char*)buf.data().data(), buf.size());
						}
					}
				}

				return ec;
			}

			virtual bool consume(flex_buffer& buffer, int ver = 0) override
			{
				this->version(ver);

				consume_cookie();

				if (this->header().deserialize(buffer))
				{
					return false;
				}

				this->body().deserialize(buffer);

				return true;
			}

			virtual bool commit_cookie(flex_buffer&) = 0;
			virtual bool consume_cookie() = 0;
		};
	} // namespace virgo
} // namespace aquarius