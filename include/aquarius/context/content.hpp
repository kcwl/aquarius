#pragma once
#include <aquarius/context/impl/context.hpp>

namespace aquarius
{
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

		virtual bool visit(std::shared_ptr<_Response> resp, std::shared_ptr<basic_session> session_ptr)
		{
			response_ptr_ = resp;

			session_ptr_ = session_ptr;

			auto result = handle();

			if (result)
			{
				invoke_callback_helper::apply(response_ptr_->uuid(), response_ptr_);
			}
			else
			{
				XLOG_ERROR() << "[context] " << this->name() << " handle error";
			}

			return result;
		}

	protected:
		virtual bool handle() = 0;

	protected:
		std::shared_ptr<_Response> response_ptr_;
	};
} // namespace aquarius