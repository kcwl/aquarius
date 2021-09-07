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
		
		template<typename T>
		int accept(std::shared_ptr<T> msg_ptr, std::shared_ptr<context> ctx_ptr)
		{
			using request_type = visitor<T, int>;

			auto request_ptr = std::dynamic_pointer_cast<request_type>(ctx_ptr);

			if(request_ptr == nullptr)
				return ctx_ptr->visit(msg_ptr);

			return request_ptr->visit(msg_ptr);
		}

		constexpr std::size_t proto()
		{
			return Number;
		}

	private:
		std::shared_ptr<header_t> header_ptr_;

		body_t body_;
	};


}
