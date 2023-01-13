#pragma once
#include <aquarius/impl/session.hpp>
#include <aquarius/impl/visitor.hpp>
#include <memory>

namespace aquarius
{
	namespace impl
	{
		class context : public impl::visitor<impl::xmessage, int>
		{
		public:
			context(const std::string& name, std::chrono::steady_clock::duration timeout)
				: name_(name)
				, timeout_(timeout)
			{}

			context(const context&) = delete;

			context(context&&) = default;

			virtual ~context() = default;

			context& operator=(const context&) = delete;

		public:
			virtual int on_connected() = 0;

			virtual int on_closed() = 0;

			virtual int on_timeout() = 0;

			virtual int visit([[maybe_unused]] impl::xmessage* msg, [[maybe_unused]] visit_mode mode) override
			{
				return 0;
			}

			void attach(std::shared_ptr<session> trans_ptr)
			{
				session_ptr_ = trans_ptr;
			}

		protected:
			virtual void on_error(int result) = 0;

		protected:
			std::shared_ptr<session> session_ptr_;

			std::string name_;

			std::chrono::steady_clock::duration timeout_;
		};

		template <typename _Request, typename _Response>
		class context_impl : public context, public impl::visitor<_Request, int>
		{
		public:
			context_impl(const std::string& name)
				: context(name, 0s)
				, request_ptr_(nullptr)
			{}

		public:
			virtual int on_connected() override
			{
				session_ptr_->close();

				return 0;
			}

			virtual int on_closed() override
			{
				session_ptr_->close();

				return 0;
			}

			virtual int on_timeout() override
			{
				return 0;
			}

			virtual int visit(_Request* req, [[maybe_unused]] visit_mode mode)
			{
				request_ptr_ = req;

				return handle();
			}

		protected:
			virtual int handle() = 0;

			virtual void on_error([[maybe_unused]] int result) override
			{}

			bool send_response(int result, std::chrono::steady_clock::duration timeout = 10s)
			{
				response_.header().clone(request_ptr_->header());

				response_.header().result_ = result;

				if (!session_ptr_->write(std::move(response_), timeout))
					return false;

				return true;
			}

		protected:
			_Request* request_ptr_;

			_Response response_;
		};
	} // namespace impl
} // namespace aquarius