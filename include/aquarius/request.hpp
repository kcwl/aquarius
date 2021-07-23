#pragma once
#include "message.hpp"
#include "header.hpp"
#include "detail/router.hpp"

namespace aquarius
{
	template<class Body, std::size_t Number>
	class request
		: public message<request_header, Body, Number>
		, public std::enable_shared_from_this<request<Body, Number>>
	{
	public:
		request() = default;
		virtual ~request() = default;

	public:
		void parse_bytes(detail::streambuf& ios)
		{
			easybuffers::convert cvt(ios);

			// 处理header
			cvt >> this->header()->proto_id_ >> this->header()->part_id_ >> this->header()->reserve_ >> this->header()->src_id_ >> this->header()->session_id_;

			// 处理body
			auto& _body = this->body();

			cvt >> _body;

			//触发ctx
			auto ctx_ptr = detail::router::instance().route_func("ctx_" + std::to_string(this->header()->proto_id_));

			if(ctx_ptr == nullptr)
				return;

			this->accept(this->shared_from_this(), ctx_ptr, this->get_conn_ptr());
		}

		void to_bytes(detail::streambuf& ios)
		{
			// 处理header
			ios << this->header()->proto_id_ << this->header()->part_id_ << this->header()->reserve_ << this->header()->src_id_ <<  this->header()->session_id_;

			// 处理body
			auto& _body = this->body();

			ios << _body;
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