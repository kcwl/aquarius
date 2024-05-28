#pragma once
#include <aquarius/context/visitor.hpp>
#include <aquarius/core/logger.hpp>
#include <chrono>

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class basic_context : public bare_visitor<basic_message>
	{
	public:
		explicit basic_context(const std::string& name)
			: visitor_(name)
			, connect_ptr_(nullptr)
		{}

		basic_context()
			: basic_context("basic context")
		{}

		virtual ~basic_context() = default;

		basic_context(const basic_context&) = default;

		basic_context(basic_context&&) = default;

		basic_context& operator=(const basic_context&) = default;

	public:
		virtual error_code visit(basic_message*, basic_connect*) override
		{
			XLOG_WARNING() << visitor_ << " maybe visit an unknown message!";

			return error_code();
		}

		std::string visitor() const
		{
			return visitor_;
		}

	public:
		virtual void on_accept()
		{
			return;
		}

	protected:
		std::string visitor_;

		basic_connect* connect_ptr_;
	};

} // namespace aquarius