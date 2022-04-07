#pragma once
#include "field.hpp"
#include "header.hpp"
#include "header_value.hpp"
#include "../stream.hpp"

#pragma warning(disable:4100)

namespace aquarius
{
	namespace msg
	{
		template<bool Request, typename Body, std::size_t Number>
		class message 
			: private header<Request,header_fields>
			, private detail::empty_value<Body>
		{
		public:
			enum { NUMBER = Number };

		public:
			using header_type = header<Request, header_fields>;
			using body_type = Body;

		public:
			message() {}

			message(header_type header)
				: header_type(header)
			{

			}

			message(message&&) = default;

			message(message const&) = default;

			message& operator=(message&&) = default;

			message& operator=(message const&) = default;

		public:
			header_type& header()noexcept 
			{
				return *this;
			}

			header_type& header() const noexcept
			{
				return *this;
			}

			Body& body() noexcept
			{
				return this->detail::empty_value<body_type>::get();
			}

			Body& body() const noexcept
			{
				return this->detail::empty_value<body_type>::get();
			}
		};
	}
}