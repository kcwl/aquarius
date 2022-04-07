#pragma once
#include "message.hpp"


namespace aquarius
{
	namespace msg
	{
		struct null_body {};

		template<bool Request, typename Body, std::size_t N>
		class null_baisc_message : public message<Request, Body, N>
		{
		public:
			null_baisc_message() = default;
			
			~null_baisc_message() = default;

		public:
			template<bool R, typename B, std::size_t N>
			null_baisc_message(const std::shared_ptr<message<R, B, N>>& msg_ptr)
			{
				this->header() = msg_ptr->header();
			}

			null_baisc_message(null_baisc_message const&) = default;

			null_baisc_message(null_baisc_message&&) = default;

			null_baisc_message& operator=(null_baisc_message const&) = default;
		};
	}

	using null_message = msg::null_baisc_message<true, msg::null_body, 0>;
}