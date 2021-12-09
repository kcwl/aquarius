#pragma once
#include "message.hpp"

namespace aquarius
{
	template<class _Body, std::size_t Number>
	class request
		: protected message<request_header, _Body, Number>
		, protected visiable<context, send_response_t>
		, public std::enable_shared_from_this<request<_Body, Number>>
	{
	public:
		request() = default;
		virtual ~request() = default;

	public:
		virtual void parse_bytes(streambuf& ios)
		{
			ios >> this->header()->proto_id_ >> this->header()->part_id_ >> this->header()->reserve_ >> this->header()->src_id_ >> this->header()->session_id_;

			this->body().parse_bytes(ios);
		}

		virtual void to_bytes(streambuf& ios)
		{
			ios << this->header()->proto_id_ << this->header()->part_id_ << this->header()->reserve_ << this->header()->src_id_ <<  this->header()->session_id_;

			this->body().to_bytes(ios);
		}

		virtual int accept(std::shared_ptr<context> ctx_ptr, send_response_t&& f)
		{
			return accept_impl(this->shared_from_this(), ctx_ptr, std::forward<send_response_t>(f));
		}

	public:
		uint32_t get_src_id() 
		{
			return this->header()->src_id_;
		}

		void set_src_id(uint32_t src_id)
		{
			this->header()->src_id_ = src_id;
		}

		uint32_t get_session_id()
		{
			return this->header()->session_id_;
		}

		void set_session_id(uint32_t session_id)
		{
			this->header()->session_id_ = session_id;
		}
	};
}