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
			context(const std::string& name)
				: name_(name)
			{
				on_connected();
			}

			context(const context&) = delete;

			context(context&&) = default;

			virtual ~context()
			{
				if (session_ptr_)
					on_closed(session_ptr_);
			}

			context& operator=(const context&) = delete;

			DEFINE_VISITOR(xmessage, int)

		public:
			virtual int on_connected() { return 0; }

			virtual int on_closed([[maybe_unused]] std::shared_ptr<session> session_ptr) { return 0; }

			virtual int on_timeout([[maybe_unused]] std::shared_ptr<session> session_ptr) { return 0; }

		protected:
			virtual void on_error([[maybe_unused]] int result){};

		protected:
			std::shared_ptr<session> session_ptr_;

			std::string name_;
		};

		template <typename _Request, typename _Response>
		class context_impl : public context, public impl::visitor<_Request, int>
		{
		public:
			context_impl(const std::string& name)
				: context(name)
				, request_ptr_(nullptr)
			{}

		public:
			virtual int on_connected() override { return 0; }

			virtual int on_closed(std::shared_ptr<session> session_ptr) override
			{
				session_ptr->close();

				return 0;
			}

			virtual int on_timeout(std::shared_ptr<session> session_ptr) override
			{
				session_ptr->close();
				return 0;
			}

			virtual int visit(_Request* req, std::shared_ptr<session> session_ptr)
			{
				request_ptr_ = req;

				session_ptr_ = session_ptr;

				return handle();
			}

		protected:
			virtual int handle() = 0;

			virtual void on_error([[maybe_unused]] int result) override {}

			bool send_response(int result)
			{
				response_.header().clone(request_ptr_->header());

				response_.header().result_ = result;

				if (!session_ptr_->write(std::move(response_)))
					return false;

				return true;
			}

		protected:
			_Request* request_ptr_;

			_Response response_;
		};
	} // namespace impl
} // namespace aquarius