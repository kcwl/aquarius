#pragma once
#include <memory>
#include "header.hpp"

namespace aquarius
{

	template<class Header, class Body, std::size_t Number>
	class message
	{
		using header_t = Header;
		using body_t = Body;

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

		auto body() ->std::shared_ptr<body_t>
		{
			return body_ptr_;
		}

	public:
		virtual std::size_t parse_bytes(iostream&&, std::size_t) = 0;

		virtual void* to_bytes() = 0;

	private:
		std::shared_ptr<header_t> header_ptr_;
		std::shared_ptr<body_t> body_ptr_;
	};

	template<class Body, std::size_t Number>
	using request = message<proto_header, Body, Number>;

	template<class Body, std::size_t Number>
	using response = message<proto_header, Body, Number>;
}
