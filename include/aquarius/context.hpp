#pragma once
#include <aquarius/system/event.hpp>
#include <aquarius/system/visitor.hpp>
#include <aquarius/session.hpp>
#include <chrono>
#include <aquarius/logger.hpp>

using namespace std::chrono_literals;

namespace aquarius
{
	class xsession;
}

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

	template <typename _Request, typename _Response>
	class context : public basic_context, public system::shared_visitor<_Request>
	{
	public:
		context(const std::string& name)
			: basic_context(name, 1s)
			, request_ptr_(nullptr)
			, response_()
		{}

	public:
		virtual void on_accept() override
		{
			// flow monitor
		}

		virtual void on_close() override
		{
			// clear
		}

		virtual void on_timeout() override
		{
			// timeout
		}

		virtual error_code visit(std::shared_ptr<_Request> req, std::shared_ptr<xsession> session_ptr, error_code& ec)
		{
			request_ptr_ = req;

			session_ptr_ = session_ptr;

			return ec = handle();
		}

	protected:
		virtual error_code handle() = 0;

		bool send_response(int result)
		{
			auto fs = make_response(result);

			this->send_request(std::move(fs));

			return true;
		}

	private:
		flex_buffer_t make_response(int32_t result)
		{
			response_.header()->result = result;

			flex_buffer_t fs;

			error_code ec{};

			response_.to_binary(fs, ec);

			return fs;
		}

	protected:
		std::shared_ptr<_Request> request_ptr_;

		_Response response_;
	};

	template <typename _Response>
	class handle : public context<_Response, int>
	{
	public:
		handle(const std::string& name)
			: context<_Response, int>(name)
		{}
	};
} // namespace aquarius

#define AQUARIUS_MESSAGE_REGIST(req) static aquarius::msg_regist<req> msg_##req(req::Number)

#define MESSAGE_MULTI_DEFINE(name_space, req)                                                                          \
	static aquarius::ctx::msg_regist<name_space::req> msg_##req(name_space::req::Number)

#define CONTEXT_DEFINE_IMPL(msg, basic_context)                                                                        \
	static aquarius::ctx_regist<basic_context> ctx_##basic_context(msg::Number)

#define CONTEXT_IMPL_MULTI_DEFINE(msg, name_space, basic_context)                                                      \
	static aquarius::ctx_regist<name_space::basic_context> ctx##basic_context(msg::Number)

#define AQUARIUS_CONTEXT_REGIST(msg, basic_context)                                                                    \
	AQUARIUS_MESSAGE_REGIST(msg);                                                                                      \
	CONTEXT_DEFINE_IMPL(msg, basic_context)

#define CONTEXT_DEFINE_MULTI_MESSAGE(name_space, req, basic_context)                                                   \
	MESSAGE_MULTI_DEFINE(name_space, req);                                                                             \
	CONTEXT_DEFINE_IMPL(name_space::req, basic_context)

#define CONTEXT_DEFINE_MULTI_CONTEXT (msg, name_space, basic_context)

#define CONTEXT_MULTI_DEFINE(msg_name_space, msg, ctx_name_space, basic_context)                                       \
	MESSAGE_MULTI_DEFINE(msg_name_space, msg);                                                                         \
	static aquarius::ctx_regist<ctx_name_space::basic_context> ctx_##basic_context(msg_name_space::msg::Number)

#define CONTEXT_SYSTEM_DEFINE(basic_context)                                                                           \
	static aquarius::ctx_regist<basic_context> ctx_system_##basic_context(basic_context::Number)