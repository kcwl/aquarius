#pragma once
#include <aquarius/context/basic_context.hpp>
#include <aquarius/invoke/invoke_callback.hpp>

namespace aquarius
{
	template <typename _Request, typename _Response>
	class context : public basic_context, public impl::shared_visitor<_Request>
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

		bool send_response(error_code result)
		{
			auto fs = make_response(result);

			this->send_request(std::move(fs));

			return true;
		}

		void make_transfer()
		{
			if (request_ptr_->header()->session_id != 0)
				return;

			request_ptr_->header()->session_id = this->session_ptr_->uuid();
		}

	private:
		flex_buffer_t make_response(error_code result)
		{
			response_.set_uuid(request_ptr_->uuid());

			response_.header()->result = static_cast<int>(result);

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
	class content : public basic_context, public impl::shared_visitor<_Response>
	{
	public:
		content(const std::string& name)
			: basic_context(name, 1s)
			, response_ptr_()
		{}

	public:
		virtual void on_accept() override
		{
			// flow monitor
		}

		virtual error_code visit(std::shared_ptr<_Response> resp, std::shared_ptr<xsession> session_ptr, error_code& ec)
		{
			response_ptr_ = resp;

			session_ptr_ = session_ptr;

			ec = handle();

			if (!ec)
			{
				invoke_callback_helper::apply(response_ptr_->uuid(), response_ptr_);
			}

			return ec;
		}

	protected:
		virtual error_code handle() = 0;

	protected:
		std::shared_ptr<_Response> response_ptr_;
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