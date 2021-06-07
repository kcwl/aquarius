#pragma once
#include "message.hpp"
#include "header.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	class response : public message<response_header, Body, Number>
	{
	public:
		response() = default;
		virtual ~response() = default;

	public:
		void set_result (int result)
		{
			this->header()->result_ = result;
		}

		int get_result()
		{
			return this->header()->result_;
		}

		virtual std::size_t parse_bytes(detail::streambuf& ios) override
		{
			// 处理header
			ios >> this->header()->proto_id_ >> this->header()->part_id_ >> this->header()->reserve_ >> this->header()->result_;

			// 处理body
			this->body()->parse_bytes(ios);

			return 0;
		}

		virtual std::size_t to_bytes(detail::streambuf& ios) override
		{
			// 处理header
			ios << this->header()->proto_id_ << this->header()->part_id_ << this->header()->reserve_ << this->header()->result_;

			// 处理body
			this->body()->to_bytes(ios);
			return 0;
		}
	};
}

