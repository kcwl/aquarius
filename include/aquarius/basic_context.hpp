#pragma once
#include <aquarius/tcp/auto_register.hpp>
#include <chrono>
#include <string>

using namespace std::chrono_literals;

namespace aquarius
{
	template <typename Message>
	class basic_context
	{
	public:
		basic_context(const std::string& name, const std::chrono::milliseconds timeout)
			: message_ptr_(nullptr)
			, ctx_name_(name)
			, timeout_(timeout)
		{}

	public:
		template <typename Session>
		int visit(std::shared_ptr<Message> msg, std::shared_ptr<Session>)
		{
			message_ptr_ = msg;

			return this->handle();
		}

		std::shared_ptr<Message> message()
		{
			return message_ptr_;
		}

	protected:
		virtual int handle() = 0;

	protected:
		std::shared_ptr<Message> message_ptr_;

		std::string ctx_name_;

		std::chrono::milliseconds timeout_;
	};
} // namespace aquarius