#pragma once
#include <memory>
#include "header.hpp"
#include "context.hpp"

namespace aquarius
{
	class basic_message {};

	template<class Header, class Body, std::size_t Number>
	class message 
		: public basic_message
	{
		using header_t = Header;
		using body_t = Body;

	public:
		message()
			: header_ptr_(new header_t())
			, body_()
		{
		}

		virtual ~message() = default;

	public:
		std::shared_ptr<header_t> header()
		{
			return header_ptr_;
		}

		body_t& body()
		{
			return body_;
		}

		constexpr std::size_t proto()
		{
			return Number;
		}

	public:
		virtual void parse_bytes(streambuf& ios) = 0;

		virtual void to_bytes(streambuf& ios) = 0;

	private:
		std::shared_ptr<header_t> header_ptr_;

		body_t body_;
	};


}
