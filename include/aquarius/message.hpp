#pragma once
#include <memory>
#include "header.hpp"
#include "context.hpp"

namespace aquarius
{
	class basic_message {};

	template<class _Header, class _Body, std::size_t Number>
	class message 
		: public basic_message
	{
	public:
		message()
			: header_ptr_(new _Header())
			, body_()
		{
		}

		virtual ~message() = default;

	public:
		std::shared_ptr<_Header> header()
		{
			return header_ptr_;
		}

		_Body& body()
		{
			return body_;
		}

		inline constexpr static std::size_t proto()
		{
			return Number;
		}

	private:
		std::shared_ptr<_Header> header_ptr_;

		_Body body_;
	};


}
