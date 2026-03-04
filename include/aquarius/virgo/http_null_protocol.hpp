#pragma once
#include <aquarius/serialize/http_json_serialize.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
#include <aquarius/ip/http/http_header.hpp>

namespace aquarius
{
	namespace virgo
	{
		class http_null_body : public http_json_serialize
		{
		public:
			http_null_body() = default;
			virtual ~http_null_body() = default;

			http_null_body(http_null_body&&) = default;

			http_null_body& operator=(http_null_body&&) = default;

			virtual void serialize(aquarius::flex_buffer& buffer) override
			{
				buffer.sputn((char*)buf_.data().data(), buf_.size());
			}

			virtual void deserialize(aquarius::flex_buffer& buffer) override
			{
				buf_.sputn((char*)buffer.data().data(), buffer.size());
			}

		public:
			flex_buffer buf_;
		};

		class http_null_request : public http_request<"", http_method::post, http_null_body> {};
		class http_null_response : public http_response<http_method::post, http_null_body> {};
	} // namespace virgo
} // namespace aquarius