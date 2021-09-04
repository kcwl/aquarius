#pragma once
#include <memory>
#include "header.hpp"
#include "context.hpp"

namespace aquarius
{
	class basic_message {};

	using eb_stream = easybuffers::ebstream<>;

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
		int accept(std::shared_ptr<T> msg_ptr, std::shared_ptr<context> ctx_ptr,std::shared_ptr<connect> conn_ptr)
		{
			using request_type = visitor<T, int>;

			auto request_ptr = std::dynamic_pointer_cast<request_type>(ctx_ptr);

			if(request_ptr == nullptr)
				return ctx_ptr->visit(msg_ptr,conn_ptr);

			return request_ptr->visit(msg_ptr,conn_ptr);
		}

		constexpr std::size_t proto()
		{
			return Number;
		}

		void set_conn_ptr(std::shared_ptr<connect> conn_ptr)
		{
			conn_ptr_ = conn_ptr;
		}

		auto get_conn_ptr()
		{
			return conn_ptr_;
		}

	private:
		std::shared_ptr<header_t> header_ptr_;

		std::shared_ptr<connect> conn_ptr_;

		body_t body_;
	};


}
