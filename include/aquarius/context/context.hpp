#pragma once
#include <aquarius/context/impl/context.hpp>

namespace aquarius
{
	template <typename _Request, typename _Response>
	class context : public basic_context, public shared_visitor<_Request, basic_session>
	{
	public:
		context(const std::string& name)
			: basic_context(name)
			, request_ptr_(nullptr)
			, response_()
		{}

	public:
		virtual void on_accept() override
		{
			// flow monitor
		}

		virtual bool visit(std::shared_ptr<_Request> req, std::shared_ptr<basic_session> session_ptr)
		{
			request_ptr_ = req;

			session_ptr_ = session_ptr;

			auto result = handle();

			if (!result)
			{
				XLOG_ERROR() << "[context] " << this->name() << " handle error";
			}

			return result;
		}

	protected:
		virtual bool handle() = 0;

		bool send_response(int result)
		{
			auto fs = make_response(result);

			this->send_request(std::move(fs));

			return true;
		}

	private:
		flex_buffer_t make_response(int result)
		{
			response_.set_uuid(request_ptr_->uuid());

			response_.header()->result = static_cast<int>(result);

			flex_buffer_t fs;

			response_.to_binary(fs);

			return fs;
		}

	protected:
		std::shared_ptr<_Request> request_ptr_;

		_Response response_;
	};
} // namespace aquarius