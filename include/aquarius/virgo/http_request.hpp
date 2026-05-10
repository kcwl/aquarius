#pragma once
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <format>

namespace aquarius
{
	template <detail::string_literal Router, typename Body, http_version Version = http_version::http1_1>
	class http_request : public basic_http_protocol<true, Body>
	{
	public:
		using base = basic_http_protocol<true, Body>;

		using base::has_request;

		constexpr static auto this_router = detail::bind_param<Router>::value;

	public:
		http_request() = default;

		virtual ~http_request() = default;

		http_request(const http_request&) = delete;
		http_request& operator=(const http_request&) = delete;

		http_request(http_request&&) noexcept = default;
		http_request& operator=(http_request&&) noexcept = default;

	protected:
		virtual void commit_command_header(flex_buffer& buffer) override
		{
			std::string get_url(this_router);

			if (this->method() == http_method::get)
			{
				this->body().set_method(static_cast<http_method>(this->method()));

				flex_buffer buf{};

				this->body().serialize(buf);

				get_url += std::string((char*)buf.data().data(), buf.size());
			}

			auto line = std::format("{} {} {}\r\n", method_to_string(static_cast<http_method>(this->method())), get_url, version_to_string(Version));

			buffer.sputn(line.c_str(), line.size());
		}
	};
} // namespace aquarius