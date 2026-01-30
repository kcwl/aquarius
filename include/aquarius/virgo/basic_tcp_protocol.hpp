#pragma once
#include <aquarius/basic_protocol.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <bool Request, typename Header, typename Body, typename Allocator = std::allocator<Body>>
		class basic_tcp_protocol : public basic_protocol<Header, std::add_pointer_t<Body>, Allocator>,
								   public header_fields
		{
			constexpr static auto __seq_number__ = 1;

		public:
			using base = basic_protocol<Header, std::add_pointer_t<Body>, Allocator>;

			using header_field_type = header_fields;

			using typename base::header_t;

			using typename base::body_t;

			constexpr static auto has_request = Request;

		public:
			basic_tcp_protocol() = default;

			virtual ~basic_tcp_protocol() = default;

			basic_tcp_protocol(const basic_tcp_protocol&) = default;

			basic_tcp_protocol& operator=(const basic_tcp_protocol& other) = default;

			basic_tcp_protocol(basic_tcp_protocol&& other) noexcept = default;

			basic_tcp_protocol& operator=(basic_tcp_protocol&& other) noexcept = default;

		public:
			bool operator==(const basic_tcp_protocol& other) const
			{
				return base::operator==(other) && header_field_type::operator==(other);
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				os << this->header() << " " << this->body();

				return os;
			}
		};
	} // namespace virgo
} // namespace aquarius