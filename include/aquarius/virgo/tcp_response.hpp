#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Body>
		class tcp_response : public basic_tcp_protocol<false, tcp_header, Body>
		{
		public:
			using base = basic_tcp_protocol<false, tcp_header, Body>;

			using base::has_request;

		public:
			tcp_response() = default;

			virtual ~tcp_response() = default;

			tcp_response(const tcp_response& other) = default;

			tcp_response& operator=(const tcp_response& other) = default;

			tcp_response(tcp_response&& other) noexcept = default;

			tcp_response& operator=(tcp_response&& other) noexcept = default;

		protected:
			virtual void commit_command_header(flex_buffer&) override { return; }
		};
	} // namespace virgo

	template <typename Body>
	struct is_message_type<virgo::tcp_response<Body>> : std::true_type
	{};
} // namespace aquarius