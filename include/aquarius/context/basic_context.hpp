#pragma once
#include <aquarius/logger.hpp>
#include <aquarius/session/xsession.hpp>
#include <aquarius/system/event.hpp>
#include <aquarius/system/visitor.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	class basic_context : public system::bare_visitor<basic_message>, public system::event_call
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
		virtual error_code visit(basic_message*, std::shared_ptr<xsession> session_ptr, error_code& ec) override
		{
			XLOG_WARNING() << name_ << " maybe visit an unknown message!";

			return ec = system_errc::invalid_message;
		}

		std::string content()
		{
			return name_;
		}

	public:
		virtual void on_accept() override
		{
			return;
		}

		virtual void on_close() override
		{
			return;
		}

		virtual void on_timeout() override
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