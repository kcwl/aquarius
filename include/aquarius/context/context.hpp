#pragma once
#include <aquarius/connect/impl/session.hpp>
#include <aquarius/context/impl/visitor.hpp>
#include <aquarius/core/logger.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class basic_context : public impl::bare_visitor<basic_message>
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
		virtual bool visit(basic_message*, std::shared_ptr<basic_session> session_ptr) override
		{
			XLOG_WARNING() << name_ << " maybe visit an unknown message!";

			return true;
		}

		std::string name()
		{
			return name_;
		}

	public:
		virtual void on_accept()
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

		std::shared_ptr<basic_session> session_ptr_;
	};

} // namespace aquarius