#pragma once
#include <aquarius/context/basic_context.hpp>
#include <aquarius/core/core.hpp>

namespace aquarius
{
	template <typename _Request, typename _Response>
	class context : public basic_context, public shared_visitor<_Request>
	{
	public:
		context(const std::string& name, const std::chrono::milliseconds& timeout = timeout_dura)
			: basic_context(name, timeout)
			, request_ptr_(nullptr)
			, response_()
		{}

	public:
		virtual error_code visit(std::shared_ptr<_Request> req, basic_connect* connect_ptr)
		{
			request_ptr_ = req;

			connect_ptr_ = connect_ptr;

			//auto future = this->template post<error_code>([&] { return this->handle(); });

			//auto status = future.wait_for(timeout_);

			//if (status != std::future_status::ready)
			//{
			//	XLOG_WARNING() << this->visitor_ << "handle timeout!";

			//	return errc::timeout;
			//}

			//try
			//{
			//	auto result = future.get();

			//	if (result)
			//	{
			//		XLOG_ERROR() << this->visitor_ << " handle error, maybe " << result.message();
			//	}

			//	return result;
			//}
			//catch (std::exception& ec)
			//{
			//	XLOG_ERROR() << ec.what();
			//}
			return this->handle();
			//return errc::ok;
		}

	protected:
		virtual error_code handle() = 0;

		bool send_response(int result)
		{
			response_.header()->set_session(request_ptr_->header()->session());
			auto fs = make_response(result);

			this->connect_ptr_->send_packet(std::move(fs));

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