#pragma once
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/default_selector.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/tcp/tcp_header.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{

		template <detail::string_literal Router, typename Body>
		class tcp_request : public basic_tcp_protocol<true, tcp_request_header, Body>
		{
		public:
			using base = basic_tcp_protocol<true, tcp_request_header, Body>;

			using base::has_request;

			constexpr static auto this_router = detail::bind_param<Router>::value;

		public:
			tcp_request() = default;

			virtual ~tcp_request() = default;

			tcp_request(const tcp_request&) = default;

			tcp_request& operator=(const tcp_request&) = default;

			tcp_request(tcp_request&&) noexcept = default;

			tcp_request& operator=(tcp_request&&) noexcept = default;

		public:
			bool operator==(const tcp_request& other) const
			{
				return base::operator==(other);
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				return base::operator<<(os);
			}

		protected:
			virtual std::string router() override
			{
				return std::string(this_router);
			}
		};

		template <detail::string_literal Router, typename Body>
		std::ostream& operator<<(std::ostream& os, const tcp_request<Router, Body>& req)
		{
			req << os;

			return os;
		}
	} // namespace virgo
	template <detail::string_literal Router, typename Body>
	struct is_message_type<virgo::tcp_request<Router, Body>> : std::true_type
	{};

	template <detail::string_literal Router, typename Body>
	struct handler_tag_traits<virgo::tcp_request<Router, Body>>
	{
		using type = tcp_protocol;

		using selector = ip::tcp_selector;
	};
} // namespace aquarius