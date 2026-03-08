#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/tcp/tcp_header.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	namespace virgo
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
				binary_parse().to_datas(this_router, buffer);
			}
		};
	} // namespace virgo
	template <detail::string_literal Router, typename Body>
	struct is_message_type<virgo::tcp_request<Router, Body>> : std::true_type
	{};
} // namespace aquarius