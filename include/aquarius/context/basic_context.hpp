#pragma once
#include <aquarius/context/impl/visitor.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/session/xsession.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace impl
	{
		class basic_message;
	}
} // namespace aquarius

namespace aquarius
{
	class basic_context : public impl::bare_visitor<impl::basic_message>
	{
	public:
		explicit basic_context(const std::string& name, std::chrono::milliseconds timeout)
			: name_(name)
			, timeout_(timeout)
		{}

		basic_context()
			: basic_context("basic context", 0s)
		{}

		virtual ~basic_context() = default;

		basic_context(const basic_context&) = delete;

		basic_context(basic_context&&) = default;

		basic_context& operator=(const basic_context&) = delete;

	public:
		virtual error_code visit(impl::basic_message*, std::shared_ptr<xsession> session_ptr, error_code& ec) override
		{
			XLOG_WARNING() << name_ << " maybe visit an unknown message!";

			return ec = system_errc::invalid_message;
		}

		std::string content()
		{
			return name_;
		}

	public:
		virtual void on_accept()
		{
			return;
		}

		virtual void on_close()
		{
			return;
		}

		virtual void on_timeout()
		{
			return;
		}

	protected:
		bool send_request(flex_buffer_t&& stream)
		{
			if (!session_ptr_)
				return false;

			session_ptr_->async_write(std::move(stream));

			return true;
		}

	protected:
		std::string name_;

		std::chrono::milliseconds timeout_;

		std::shared_ptr<xsession> session_ptr_;
	};

} // namespace aquarius