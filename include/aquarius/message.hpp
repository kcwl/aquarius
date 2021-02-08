#pragma once
#include <memory>
#include "header.hpp"
#include "detail/streambuf.hpp"

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
		constexpr std::size_t proto()
		{
			return Number;
		}


	public:
	public:
		std::size_t parse_bytes(const detail::streambuf& ios) override
		{
			//处理header

			//处理body

			return 0;
		}

		std::size_t to_bytes(const detail::streambuf& ios)
		{
			//转换header

			//处理body
			return 0;
		}

	private:
		std::shared_ptr<header_t> header_ptr_;
		std::shared_ptr<body_t> body_ptr_;
	};
}
