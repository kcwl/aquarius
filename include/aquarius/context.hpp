#pragma once
#include <aquarius/detail/event.hpp>
#include <aquarius/detail/visitor.hpp>
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
	class basic_context : public detail::bare_visitor<basic_message, int>, public detail::event_call
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
		virtual int visit(basic_message*, std::shared_ptr<xsession> session_ptr) override
		{
			XLOG(warning) << name_ << " maybe visit an unknown message!";

			return int{};
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
	class context : public basic_context, public detail::shared_visitor<_Request, int>
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

		virtual int visit(std::shared_ptr<_Request> req, std::shared_ptr<xsession> session_ptr)
		{
			request_ptr_ = req;

			session_ptr_ = session_ptr;

			return handle();
		}

	protected:
		virtual int handle() = 0;

		bool send_response(int result)
		{
			auto fs = make_response(result);

			this->send_request(std::move(fs));

			return true;
		}

	private:
		flex_buffer_t make_response(int32_t result)
		{
			response_.header()->clone(request_ptr_->header());

			response_.header()->result = result;

			flex_buffer_t fs;

			response_.to_binary(fs);

			return fs;
		}

	protected:
		std::shared_ptr<_Request> request_ptr_;

		_Response response_;
	};

	template <typename _Response>
	class client_context : public context<_Response, int>
	{
	public:
		client_context(const std::string& name)
			: context<_Response, int>(name)
		{}
	};

	template <std::size_t Number>
	class system_context : public basic_context
	{
		static_assert(Number >= 500 && Number < 1000, "system context number must be 500 ~ 999");

	public:
		constexpr static std::size_t Number = Number;

	public:
		system_context(const std::string& name)
			: basic_context(name, 1s)
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