#pragma once
#include <cstddef>
#include <future>
#include "common.hpp"

namespace aquarius
{
	template<typename Connect>
	class async_control
	{
	public:
		virtual ~async_control()
		{
		}

	public:
		template<typename T>
		void process(std::shared_ptr<Connect> conn_ptr, streambuf& stream)
		{
			conn_ptr_ = conn_ptr;

			return complete(stream);
		}

		virtual void complete(streambuf& stream) = 0;

		template<typename Response>
		void send_response(Response&& resp)
		{
			conn_ptr_->async_write_some(resp);
		}

	private:
		std::shared_ptr<Connect> conn_ptr_;
	};
}