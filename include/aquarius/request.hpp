#pragma once
#include "message.hpp"
#include "header.hpp"

namespace aquarius
{
	namespace aqnet
	{
		template<class Body, std::size_t Number>
		class tcp_request : public detail::request<tcp_request_header, Body, Number>
		{
		public:
			virtual std::size_t parse_bytes(iostream&& stream, std::size_t length) override
			{
				this->header()->parse_bytes(std::forward<iostream>(stream), length);

				if(this->header()->length_ + sizeof(tcp_request_header) != length)
					return -1;

				if(this->header()->protocol_ == 0)
					return -1;

				auto size = sizeof(uint32_t);
				length -= size;

				if(this->header()->sub_proto_ == 0)
					return -1;

				length -= size * 2;

				if(length == 0)
					return 0;

				return this->body()->parse_bytes(std::forward<iostream>(stream), length);
			}

			virtual void* to_bytes()
			{
				return nullptr;
			}
		};

		template<class Body, std::size_t Number>
		class http_request : public detail::request<http_request_header, Body, Number>
		{

		};
	}
}