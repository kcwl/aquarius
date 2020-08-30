#pragma once
#include <memory>
#include "message_body.hpp"

namespace aquarius
{
	namespace detail
	{
		class basic_message
		{
		public:
			basic_message() { }
		};

		template<class Header, class Body, std::size_t Number>
		class message : public basic_message
		{
			using header_t = Header;
			using body_t = message_body<Body>;

		public:
			message()
				: header_ptr_(new header_t())
				, body_ptr_(new body_t())
			{
			}

			virtual ~message() = default;

		public:
			auto header() ->std::shared_ptr<header_t>
			{
				return header_ptr_;
			}

			auto body() ->std::shared_ptr<Body>
			{
				return body_ptr_->body();
			}

		public:
			virtual std::size_t parse_bytes(iostream&&, std::size_t) = 0;

			virtual void* to_bytes() = 0;

		private:
			std::shared_ptr<header_t> header_ptr_;
			std::shared_ptr<body_t> body_ptr_;
		};

		template<class Header, class Body, std::size_t Number>
		using request = message<Header, Body, Number>;

		template<class Header, class Body, std::size_t Number>
		using response = message<Header, Body, Number>;
	}
}
