#pragma once
#include <aquarius/context/context.hpp>

namespace aquarius
{
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
		virtual void on_accept() override
		{
			// flow monitor
		}

		virtual void on_close() override
		{
			// clear
		}

		virtual void on_connect() override
		{
			// log record
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

			session_ptr_->async_write(std::move(fs));

			return true;
		}

		bool send_broadcast(bool has_request = true)
		{
			session_manager::instance().broadcast(std::move(has_request ? transfer_request() : make_response(1)));

			return true;
		}

		template <typename _Message>
		bool send_broadcast(_Message&& msg)
		{
			flex_buffer_t fs{};
			msg.to_binary(fs);

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
			msg.to_binary(fs);

			session_manager::instance().broadcast(std::move(fs), std::forward<_Func>(f));

			return true;
		}

	private:
		flex_buffer_t make_response(int32_t result)
		{
			response_.header().clone(request_ptr_->header());

			response_.header().result_ = result;

			flex_buffer_t fs;

			response_.to_binary(fs);

			return fs;
		}

		flex_buffer_t transfer_request()
		{
			flex_buffer_t fs;

			request_ptr_->to_binary(fs);

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
}