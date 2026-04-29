#pragma once
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/tcp_header.hpp>

namespace aquarius
{
	template <detail::string_literal Router, typename Body>
	class tcp_request : public basic_tcp_protocol<true, tcp_header, Body>
	{
	public:
		using base = basic_tcp_protocol<true, tcp_header, Body>;

		using base::has_request;

		constexpr static auto this_router = detail::bind_param<Router>::value;

	public:
		tcp_request() = default;

		virtual ~tcp_request() = default;

		tcp_request(const tcp_request&) = delete;

		tcp_request& operator=(const tcp_request&) = delete;

		tcp_request(tcp_request&&) noexcept = default;

		tcp_request& operator=(tcp_request&&) noexcept = default;

	protected:
		virtual void commit_command_header(flex_buffer& buffer) override
		{
			binary_parse().to_datas(this->src(), buffer);
			binary_parse().to_datas(this_router, buffer);
		}
	};
} // namespace aquarius