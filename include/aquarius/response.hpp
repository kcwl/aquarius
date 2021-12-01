#pragma once
#include "message.hpp"
#include "header.hpp"

namespace aquarius
{
	template<class _Body, std::size_t Number>
	class response : public message<response_header, _Body, Number>
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

		virtual void parse_bytes(streambuf& ios)
		{
			ios >> this->header()->proto_id_ >> this->header()->part_id_ >> this->header()->reserve_ >> this->header()->result_;

			this->body().parse_bytes(ios);
		}

		virtual void to_bytes(streambuf& ios)
		{
			// 处理header
			ios << this->header()->proto_id_ << this->header()->part_id_ << this->header()->reserve_ << this->header()->result_;

			// 处理body
			this->body().to_bytes(ios);
		}
	};
}

