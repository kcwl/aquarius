#pragma once
#include <aquarius/context/basic_context.hpp>
#include <aquarius/core/core.hpp>

namespace aquarius
{
	template <typename _Response>
	class content : public basic_context, public shared_visitor<_Response>
	{
	public:
		content(const std::string& name, const std::chrono::milliseconds& timeout = timeout_dura)
			: basic_context(name, timeout)
			, response_ptr_()
		{}

	public:
		virtual error_code visit(std::shared_ptr<_Response> resp, basic_connect* connect_ptr)
		{
			response_ptr_ = resp;

			connect_ptr_ = connect_ptr;

			auto future = this->template post<error_code>([&] { return this->handle(); });

			auto status = future.wait_for(timeout_);

			if (status != std::future_status::ready)
			{
				XLOG_WARNING() << this->visitor_ << "handle timeout!";

				return errc::timeout;
			}

			auto result = future.get();

			if (!result)
			{
				invoke_callback_helper::apply(response_ptr_->uuid(), response_ptr_);
			}
			else
			{
				XLOG_ERROR() <<this-> visitor_ << " handle error, maybe " << result.message();
			}

			return result;
		}

	protected:
		virtual error_code handle() = 0;

	protected:
		std::shared_ptr<_Response> response_ptr_;
	};
} // namespace aquarius