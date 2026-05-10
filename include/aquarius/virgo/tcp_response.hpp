#pragma once
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	template <detail::string_literal Router, typename Body>
	class tcp_response : public basic_tcp_protocol<false, tcp_header, Body>
	{
	public:
		using base = basic_tcp_protocol<false, tcp_header, Body>;

		using base::has_request;

		constexpr static auto this_router = detail::bind_param<Router>::value;

	public:
		tcp_response() = default;

		virtual ~tcp_response() = default;

		tcp_response(const tcp_response& other) = delete;

		tcp_response& operator=(const tcp_response& other) = delete;

		tcp_response(tcp_response&& other) noexcept = default;

		tcp_response& operator=(tcp_response&& other) noexcept = default;

	protected:
		virtual void commit_command_header(flex_buffer& buffer) override
		{
			binary_parse parse{};

			parse.to_datas(this->result(), buffer);
		}
	};
} // namespace aquarius