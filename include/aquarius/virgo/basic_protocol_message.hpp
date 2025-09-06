#pragma once
#include <aquarius/virgo/basic_message.hpp>
#include <expected>
#include <span>

namespace aquarius
{
	namespace virgo
	{
		template <bool IsRequest, typename Logic, template <bool> typename Header, typename Body, typename Parse,
				  typename Alloca = std::allocator<Body>>
		class basic_protocol_message : public basic_message<std::add_pointer_t<Body>, Alloca>, public Header<IsRequest>
		{
			using base_header = Header<IsRequest>;

		public:
			using header_t = Logic;

			using body_ptr_t = std::add_pointer_t<Body>;

		public:
			basic_protocol_message() = default;

		public:
			header_t& header()
			{
				return header_;
			}

			const header_t& header() const
			{
				return header_;
			}

		protected:
			header_t header_;

			Parse parse_;
		};
	} // namespace virgo
} // namespace aquarius