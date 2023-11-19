#pragma once
#include <aquarius/session/session_manager.hpp>

#define MESSAGE_DEFINE(req) static aquarius::msg_regist<req> msg_##req(req::Number)

#define MESSAGE_MULTI_DEFINE(name_space, req)                                                                          \
	static aquarius::ctx::msg_regist<name_space::req> msg_##req(name_space::req::Number)

#define CONTEXT_DEFINE_IMPL(msg, context) static aquarius::ctx_regist<context> ctx_##context(msg::Number)

#define CONTEXT_IMPL_MULTI_DEFINE(msg, name_space, context)                                                            \
	static aquarius::ctx_regist<name_space::context> ctx##context(msg::Number)

#define CONTEXT_DEFINE(msg, context)                                                                                   \
	MESSAGE_DEFINE(msg);                                                                                               \
	CONTEXT_DEFINE_IMPL(msg, context)

#define CONTEXT_DEFINE_MULTI_MESSAGE(name_space, req, context)                                                         \
	MESSAGE_MULTI_DEFINE(name_space, req);                                                                             \
	CONTEXT_DEFINE_IMPL(name_space::req, context)

#define CONTEXT_DEFINE_MULTI_CONTEXT (msg, name_space, context)

#define CONTEXT_MULTI_DEFINE(msg_name_space, msg, ctx_name_space, context)                                             \
	MESSAGE_MULTI_DEFINE(msg_name_space, msg);                                                                         \
	static aquarius::ctx_regist<ctx_name_space::context> ctx_##context(msg_name_space::msg::Number)

namespace aquarius
{
	class context : public detail::visitor<xmessage, int>
	{
	public:
		context(const std::string& name, std::chrono::milliseconds timeout)
			: name_(name)
			, timeout_(timeout)
		{}

		virtual ~context() = default;

		context(const context&) = delete;

		context(context&&) = default;

		context& operator=(const context&) = delete;

	public:
		virtual void on_create() = 0;

		virtual void on_close() = 0;

		virtual void on_connect() = 0;

		virtual void on_timeout() = 0;

		virtual int visit(xmessage* msg) = 0;

	protected:
		std::string name_;

		std::chrono::milliseconds timeout_;

		std::shared_ptr<xsession> session_ptr_;
	};

	template <typename _Request, typename _Response>
	class xhandle : public context, public detail::visitor<_Request, int>
	{
	public:
		xhandle(const std::string& name)
			: context(name, 1s)
			, request_ptr_(nullptr)
			, response_()
		{}

	public:
		virtual void on_create()
		{
			//flow monitor
		}

		virtual void on_close()
		{
			//clear
		}

		virtual void on_connect()
		{
			// log record
		}

		virtual void on_timeout()
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

			session_ptr_->async_write(std::move(fs));

			return true;
		}

		bool send_broadcast(bool has_request = true)
		{
			session_manager::instance().broadcast(
				std::move(has_request ? transfer_request() : make_response(1)));

			return true;
		}

		template <typename _Message>
		bool send_broadcast(_Message&& msg)
		{
			flex_buffer_t fs{};
			msg.visit(fs, visit_mode::output);

			session_manager::instance().broadcast(std::move(fs));

			return true;
		}

		template <typename _Func>
		bool send_broadcast_if(_Func&& f)
		{
			auto fs = this->transfer_request();

			session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));

			return true;
		}

		template <typename _Message, typename _Func>
		bool send_broadcast_if(_Message&& msg, _Func&& f)
		{
			flex_buffer_t fs{};
			msg.visit(fs, visit_mode::output);

			session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));

			return true;
		}

	private:
		flex_buffer_t make_response(int32_t result)
		{
			response_.header().clone(request_ptr_->header());

			response_.header().result_ = result;

			flex_buffer_t fs;

			response_.visit(fs, visit_mode::output);

			return fs;
		}

		flex_buffer_t transfer_request()
		{
			flex_buffer_t fs;

			request_ptr_->visit(fs, visit_mode::output);

			return fs;
		}

	protected:
		std::shared_ptr<_Request> request_ptr_;

		_Response response_;
	};

	template <typename _Response>
	class client_context : public xhandle<_Response, int>
	{
	public:
		client_context(const std::string& name)
			: xhandle<_Response, int>(name)
		{}

	protected:
		virtual int handle()
		{
			return 0;
		}
	};

} // namespace aquarius