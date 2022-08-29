#pragma once
#include <memory>

namespace aquarius
{
	template<typename _Request, typename _Response>
	class message_service
	{
	public:
		message_service()
			: request_ptr_(new _Request{})
			, response_()
		{

		}

		virtual ~message_service() = default;

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

		auto& request() noexcept
		{
			return request_ptr_;
		}

		auto& response() noexcept
		{
			return response_;
		}

	private:
		std::shared_ptr<_Request> request_ptr_;

		_Response response_;
	};

	template<typename _Session, typename _Request, typename _Response>
	class context
		: public message_service<_Request, _Response>
	{
	public:
		explicit context(std::shared_ptr<_Session> session_ptr)
			: session_ptr_(session_ptr)
		{

		}

		context(const context&) = delete;

		context(context&&) = default;

		virtual ~context() = default;

		context& operator=(const context&) = delete;

	public:
		template<typename _Stream>
		int accept(_Stream& stream)
		{
			this->serialize(stream);

			if (!handle())
				return 0;

			send_response(status_);

			return 1;
		}

		void send_response(uint32_t status)
		{
			if (!session_ptr_)
				return;

			session_ptr_->queue_packet(this->response());
		}

		virtual bool handle() = 0;

	private:
		std::shared_ptr<_Session> session_ptr_;

		uint32_t status_;
	};
}