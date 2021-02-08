#pragma once
#include <memory>
#include <future>
#include "detail/streambuf.hpp"

namespace aquarius
{
	namespace net
	{
		template<class Request, class Response>
		class context
		{
		public:
			context() 
				: request_ptr_(new Request{})
				, resp_()
			{ }

			virtual ~context() = default;

		public:
			int attach(const detail::streambuf& stream, detail::streambuf& os_buf)
			{
				msg_future_ = std::async(&context::message_proc,this, stream);

				attach_context();

				return resp_.to_bytes(os_buf);
			}

			int attach_context()
			{
				msg_future_.get();

				if(!handle())
					return 0;

				return 1;
			}

			virtual bool handle() = 0;

		private:
			void message_proc(const detail::streambuf& stream)
			{
				request_ptr_->parse_bytes(stream);
			}

		public:
			std::shared_ptr<Request> request_ptr_;

			Response resp_;

			std::future<void> msg_future_;
		};
	}
}