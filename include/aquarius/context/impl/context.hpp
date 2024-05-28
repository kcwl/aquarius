#pragma once
#include <aquarius/core/logger.hpp>
#include <aquarius/core/visitor.hpp>

namespace aquarius
{
	class basic_message;

	class basic_connect;
} // namespace aquarius

namespace aquarius
{
	class basic_context : public bare_visitor<basic_message, basic_connect>
	{
	public:
		explicit basic_context(const std::string& name)
			: visitor_(name)
		{}

		basic_context()
			: basic_context("basic context")
		{}

		virtual ~basic_context() = default;

		basic_context(const basic_context&) = delete;

		basic_context(basic_context&&) = default;

		basic_context& operator=(const basic_context&) = delete;

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