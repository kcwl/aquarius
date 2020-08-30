#pragma once
#include <cstdint>

namespace aquarius
{
	namespace aqnet
	{
		class basic_header
		{
		public:
			std::size_t parse_bytes(iostream&& stream, std::size_t length)
			{
				protocol_ = 10001;
				sub_proto_ = -1;
				length_ = 12;

				return parse_impl(std::forward<iostream>(stream), length) + sizeof(uint32_t) * 3;
			}

			virtual void* to_bytes() = 0;

			virtual std::size_t parse_impl(iostream&&, std::size_t) = 0;

		public:
			uint32_t protocol_;
			uint32_t sub_proto_;
			uint32_t length_;
		};

		class tcp_request_header : public basic_header
		{
		public:
			virtual std::size_t parse_impl(iostream&& stream, std::size_t length) override
			{
				stream >> src_id_ >> session_id_;

				return 8;
			}

			virtual void* to_bytes() override
			{
				return nullptr;
			}

		public:
			uint32_t src_id_;
			uint32_t session_id_;
		};

		class tcp_response_header : public basic_header
		{
		public:
			virtual std::size_t parse_impl(iostream&&, std::size_t) override
			{
				return 0;
			}

			virtual void* to_bytes() override
			{
				return this;
			}

		public:
			uint32_t result_;
		};

		class http_request_header : public basic_header
		{
		};

		class http_response_header : public basic_header
		{
		};
	}
}