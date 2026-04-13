#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <ranges>
#include <span>

namespace aquarius
{
	template <bool Request, http_method Method, typename Body, typename Allocator = std::allocator<Body>>
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

			this->body().set_method(Method);

			if constexpr (Method != http_method::get)
			{
				this->body().serialize(buf);

				if (buf.size() > 2)
				{
					this->header().content_length(buf.size());
				}
			}

			auto ec = this->header().serialize(buffer);

			if (!ec)
			{
				if constexpr (Method != http_method::get)
				{
					if (this->header().content_length() != 0)
					{
						buffer.sputn((char*)buf.data().data(), buf.size());
					}
				}
			}

			return ec;
		}

		virtual bool consume(flex_buffer& buffer) override
		{
			this->body().set_method(Method);

			if (this->header().deserialize(buffer))
			{
				return false;
			}

			this->body().deserialize(buffer);

			return true;
		}
	};
} // namespace aquarius