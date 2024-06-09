#pragma once
#include <aquarius/context/async_handle.hpp>
#include <aquarius/context/visitor.hpp>
#include <aquarius/core/logger.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class basic_context : public bare_visitor<basic_message>, public async_handle
	{
	public:
		explicit basic_context(const std::string& name, const std::chrono::milliseconds& timeout)
			: visitor_(name)
			, connect_ptr_(nullptr)
			, timeout_(timeout)
		{}

		basic_context()
			: basic_context("basic context", 0s)
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

	protected:
		std::string visitor_;

		basic_connect* connect_ptr_;

		std::chrono::milliseconds timeout_;
	};

} // namespace aquarius