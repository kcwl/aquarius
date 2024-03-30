#pragma once
#include <aquarius/context/impl/context.hpp>
#include <aquarius/client/callback.hpp>

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
} // namespace aquarius