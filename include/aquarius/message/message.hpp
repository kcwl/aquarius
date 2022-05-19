#pragma once
#include "../detail/empty_value.hpp"
#include "null_message.hpp"
#include "parse_packet.hpp"

#pragma warning(disable:4100)

namespace aquarius
{
	class context;

	namespace msg
	{
		template<bool Request, typename Header, typename Body, std::size_t Number>
		class message
			: public null_message
			, private Header
			, private aquarius::detail::empty_value<Body>
			, public std::enable_shared_from_this<message<Request,Header,Body,Number>>
		{
		public:
			enum { NUMBER = Number };

			using header_type = Header::value_type;
			using body_type = aquarius::detail::empty_value<Body>;

		public:
			message() = default;

			message(message&&) = default;

			message(message const&) = default;

			message& operator=(message&&) = default;

			message& operator=(message const&) = default;

			message& operator=(const header_type& header)
			{
				header_type::operator=(header);

				return *this;
			}

		public:
			auto& header() noexcept
			{
				return *this;
			}

			auto& header() const noexcept
			{
				return *this;
			}

			Body& body() noexcept
			{
				return body_type::get();
			}

			Body& body() const noexcept
			{
				return body_type::get();
			}

			virtual int accept(std::shared_ptr<context> ctx_ptr)
			{
				return accept_impl<null_message>(this->shared_from_this(), ctx_ptr);
			}

			virtual bool parse_bytes(aquarius::ftstream& archive)
			{
				return parse<0>::parse_bytes(this->shared_from_this(), archive);
			}

			virtual bool to_bytes(aquarius::ftstream& archive)
			{
				return parse<0>::to_bytes(this->shared_from_this(), archive);
			}
		};
	}
}