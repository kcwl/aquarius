#pragma once
#include <aquarius/context/impl/context.hpp>

namespace aquarius
{
	template <typename _Response>
	class content : public basic_context, public shared_visitor<_Response, basic_connect>
	{
	public:
		content(const std::string& name)
			: basic_context(name)
			, response_ptr_()
		{}

	public:
		virtual void on_accept() override
		{
			// flow monitor
		}

		virtual error_code visit(std::shared_ptr<_Response> resp, basic_connect* connect_ptr)
		{
			response_ptr_ = resp;

			connect_ptr_ = connect_ptr;

			auto result = handle();

			if (result)
			{
				invoke_callback_helper::apply(response_ptr_->uuid(), response_ptr_);
			}
			else
			{
				XLOG_ERROR() << "[context] " << this->visitor() << " handle error";
			}

			return result;
		}

	protected:
		virtual error_code handle() = 0;

	protected:
		std::shared_ptr<_Response> response_ptr_;
	};
} // namespace aquarius