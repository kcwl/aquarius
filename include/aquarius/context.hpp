#pragma once
#include <memory>
#include <aquarius/core/visitor.hpp>

namespace aquarius
{
	//template<typename _Request, typename _Response>
	//class message_service
	//{
	//public:
	//	message_service()
	//		: request_ptr_(new _Request{})
	//		, response_()
	//	{

	//	}

	//	virtual ~message_service() = default;

	//public:
	//	template<typename _StreamBuf>
	//	bool serialize(_StreamBuf& stream)
	//	{
	//		return request_ptr_->parse_message(stream);
	//	}

	//	template<typename _StreamBuf>
	//	bool deserialize(_StreamBuf& stream)
	//	{
	//		return response_.to_message(stream);
	//	}

	//	auto request() noexcept
	//	{
	//		return request_ptr_;
	//	}

	//	auto& response() noexcept
	//	{
	//		return response_;
	//	}

	//private:
	//	std::shared_ptr<_Request> request_ptr_;

	//	_Response response_;
	//};

	//template<typename _Request, typename _Response>
	class context
		//: public message_service<_Request, _Response>
		: public core::visitor<proto::xmessage, int>
	{
	public:
		explicit context()
		{

		}

		context(const context&) = delete;

		context(context&&) = default;

		virtual ~context() = default;

		context& operator=(const context&) = delete;

	public:
		template<typename _StreamBuf>
		int accept(_StreamBuf& stream)
		{
			//if (!this->serialize(stream))
			//	return 0;

			if (!handle())
				return 0;

			send_response(status_);

			return 1;
		}

		void send_response(uint32_t status)
		{
			//if (!session_ptr_)
				return;

			//session_ptr_->queue_packet(this->response());
		}

		virtual bool handle() = 0;

	private:
		uint32_t status_;
	};
}