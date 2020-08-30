#pragma once
#include <memory>
#include <future>
#include <aquarius/qualify.hpp>

namespace aquarius
{
	namespace aqnet
	{
		class basic_context 
		{
		public:
			int process(iostream&& stream)
			{
				return attach(std::forward<iostream>(stream));
			}

		private:
			virtual int attach(iostream&& stream) = 0;
		};

		template<class Request, class Response>
		class context : public basic_context
		{
		public:
			virtual ~context() = default;

		public:
			virtual int attach(iostream&& stream) override
			{
				msg_future_ = std::async(&context::message_proc,this, std::forward<iostream>(stream));

				return attach_context();
			}

			int attach_context()
			{
				msg_future_.get();

				if(!handle())
					return 0;

				return send_response();
			}

			int send_response()
			{
				return 1;
			}

			virtual bool handle() = 0;

		private:
			void message_proc(iostream&& stream)
			{
				//login_request lr;
				//lr.parse_bytes(stream, 36);

				//promise.set_value(lr);
				request_ptr_->parse_bytes(std::forward<iostream>(stream),0);
			}

		public:
			std::shared_ptr<Request> request_ptr_;

			Response resp_;

			std::future<void> msg_future_;
		};
	}
}