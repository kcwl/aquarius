#pragma once
#include <memory>

namespace aquarius
{
	template<typename _Request, typename _Response>
	class service
	{
	public:
		service()
			: request_ptr_(new _Request{})
			, response_()
		{

		}

		virtual ~service() = default;

	public:
		template<typename _Stream>
		void serialize(_Stream& stream)
		{
			request_ptr_->parse_message(stream);
		}

		template<typename _Stream>
		void deserialize(_Stream& stream)
		{
			response_.to_message(stream);
		}

	public:
		std::shared_ptr<_Request> request_ptr_;
		_Response response_;
	};

	template<typename _Request, typename _Ty>
	class vst
	{
	public:
		virtual int visit(std::shared_ptr<_Request> request_ptr, _Ty) = 0;
	};


	template<typename _Session, typename _Request, typename _Response>
	class context
		: public service<_Request, _Response>
		, vst<_Request, long>
	{
	public:
		context() = default;

		context(const context&) = delete;

		context(context&&) = default;

		virtual ~context() = default;

		context& operator=(const context&) = delete;

		void attach(_Session* session_ptr)
		{
			session_ptr_ = session_ptr;
		}

	public:
		virtual int visit(std::shared_ptr<_Request> request_ptr, long) override
		{
			return 0;
		}

		void send_response(uint32_t status)
		{
			if (!session_ptr_)
				return;

			session_ptr_->queue_packet(std::move(this->response_));
		}

	private:
		_Session* session_ptr_;
	};


	template<typename _Session, typename _Request, typename _Response>
	class handler
		: public context<_Session, _Request, _Response>
		, public vst<_Request, int>
	{
	public:
		handler(_Session* session_ptr)
			: context<_Session, _Request, _Response>(session_ptr)
		{

		}

	public:
		virtual int visit(std::shared_ptr<_Request> request_ptr, int) override
		{
			if (!request_ptr)
				return visit(request_ptr, 0L);

			this->request_ptr_.swap(request_ptr);

			if (!handle())
				return 0;

			this->send_response(status_);
		}

		void set_status(uint32_t status)
		{
			status_ = status;
		}

		virtual bool handle() = 0;

	private:
		uint32_t status_;
	};
}