#pragma once
#include <aquarius/context/impl/context.hpp>

namespace aquarius
{
	template <typename _Request, typename _Response>
	class context : public basic_context, public shared_visitor<_Request>
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

		virtual error_code visit(std::shared_ptr<_Request> req, basic_connect* session_ptr)
		{
			request_ptr_ = req;

			connect_ptr_ = session_ptr;

			auto result = handle();

			if (!result)
			{
				XLOG_ERROR() << this->visitor() << " handle error";
			}

			return result;
		}

	protected:
		virtual error_code handle() = 0;

		bool send_response(int result)
		{
			auto fs = make_response(result);
			if (!this->connect_ptr_)
				return false;

			this->connect_ptr_->async_write(std::move(fs));

			return true;
		}

	private:
		flex_buffer_t make_response(int result)
		{
			response_.set_uuid(request_ptr_->uuid());

			response_.header()->set_result(result);

			flex_buffer_t fs;

			error_code ec{};

			response_.to_binary(fs, ec);

			return fs;
		}

	protected:
		std::shared_ptr<_Request> request_ptr_;

		_Response response_;
	};
} // namespace aquarius