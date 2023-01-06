#pragma once
#include <aquarius/impl/session.hpp>
#include <aquarius/impl/visitor.hpp>
#include <memory>

namespace aquarius
{
	class context : public impl::visitor<impl::xmessage, int>
	{
	public:
		context(std::shared_ptr<impl::session> session_ptr)
			: session_ptr_(session_ptr)
		{}

		context(const context&) = delete;

		context(context&&) = default;

		virtual ~context() = default;

		context& operator=(const context&) = delete;

	public:
		virtual int on_connected() = 0;

		virtual int on_closed(std::shared_ptr<impl::impl>) = 0;

		virtual int on_timeout() = 0;

		virtual bool handle() = 0;

	protected:
		virtual void on_error(int result) = 0;

	protected:
		std::shared_ptr<impl::session> session_ptr_;
	};

	template <typename _Request, typename _Response>
	class context_impl : public context, public impl::visitor<_Request, int>
	{
	public:
		context_impl(const std::string& name)
			: context(name, 0)
		{}

	public:
		virtual int on_connected(std::shared_ptr<impl::session> session_ptr) override
		{
			return session_ptr->close();
		}

		virtual int on_closed(std::shared_ptr<impl::session> session_ptr) override
		{
			return session_ptr->close();
		}

		virtual int on_timeout() override
		{
			return 0;
		}

		virtual int visit(std::shared_ptr<impl::xmessage> msg)
		{
			return 0;
		}

		virtual int visit(std::shared_ptr<_Request> req)
		{
			request_ptr_.swap(req);

			return handle();
		}

	protected:
		virtual int handle() = 0;

		virtual void on_error(int result) override
		{}

		bool send_response(int result, int timeout = 1000)
		{
			response_.header() = request_ptr_->header();
			response_.set_result(result);

			if (!session_ptr_->write(&response_, timeout))
				return false;

			return true;
		}

	protected:
		std::shared_ptr<_Request> request_ptr_;

		_Response response_;
	};
} // namespace aquarius