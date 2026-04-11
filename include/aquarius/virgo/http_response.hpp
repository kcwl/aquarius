#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	template <http_method Method, typename Body, http_version Version = http_version::http1_1>
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

			if (buf.size() > 2)
			{
				this->header().content_length(buf.size());
			}

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
			std::string headerline = std::format("{} {} {}\r\n", version_to_string(Version),
												 static_cast<int>(this->result()), from_status_string(this->result()));

			buffer.sputn(headerline.c_str(), headerline.size());
		}
	};

	template <http_method Method, typename Body, http_version Version>
	struct is_message_type<http_response<Method, Body, Version>> : std::true_type
	{};
} // namespace aquarius