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
	template <bool Request, detail::string_literal Router, typename Body>
	class basic_http_protocol : public basic_tcp_protocol<Request, Router, http_header, Body>
	{
	public:
		using base_type = basic_tcp_protocol<Request, Router, http_header, Body>;

		using base_type::has_request;

	public:
		basic_http_protocol()
			: method_()
		{}

		virtual ~basic_http_protocol() = default;

		basic_http_protocol(const basic_http_protocol& other)
			: base_type(std::move(other))
			, method_(other.method_)
		{}

		basic_http_protocol(basic_http_protocol&& other)
			: base_type(std::move(other))
			, method_(std::exchange(other.method_, {}))
		{}

		basic_http_protocol& operator=(const basic_http_protocol& other) noexcept
		{
			if (std::addressof(other) != this)
			{
				base_type::operator=(std::move(other));
				method_ = other.method_;
			}

			return *this;
		}

		basic_http_protocol& operator=(basic_http_protocol&& other) noexcept
		{
			if (std::addressof(other) != this)
			{
				base_type::operator=(std::move(other));
				method_ = std::exchange(other.method_, {});
			}

			return *this;
		}

	public:
		virtual error_code commit(flex_buffer& buffer) override
		{
			error_code ec{};

			try
			{
				std::size_t bytes = this->body().byte_size();

				if (this->method() != http_method::get && bytes != 0)
				{
					this->header().content_length(bytes);
				}

				this->header().serialize(buffer);

				if (this->method() != http_method::get && bytes != 0)
				{
					this->body().serialize(buffer);
				}
			}
			catch (...)
			{
				ec = ip::error::commit;
			}

			return ec;
		}

		virtual error_code consume(flex_buffer& buffer) override
		{
			error_code ec{};

			try
			{
				this->header().deserialize(buffer);

				this->body().deserialize(buffer);
			}
			catch (...)
			{
				ec = ip::error::consume;
			}

			return ec;
		}

	public:
		void method(http_method m)
		{
			method_ = m;
		}

		http_method& method()
		{
			return method_;
		}

		const http_method& method() const
		{
			return method_;
		}

	private:
		http_method method_;
	};
} // namespace aquarius