#pragma once
#include <aquarius/detail/context_base.hpp>

namespace aquarius
{
	// template <typename Message>
	// class basic_context
	//{
	// public:
	//	basic_context(const std::string& name, const std::chrono::milliseconds timeout)
	//		: message_ptr_(nullptr)
	//		, ctx_name_(name)
	//		, timeout_(timeout)
	//	{}

	// public:
	//	template <typename Session>
	//	int visit(std::shared_ptr<Message> msg, std::shared_ptr<Session>)
	//	{
	//		message_ptr_ = msg;

	//		return this->handle();
	//	}

	//	std::shared_ptr<Message> message()
	//	{
	//		return message_ptr_;
	//	}

	// protected:
	//	virtual int handle() = 0;

	// protected:
	//	std::shared_ptr<Message> message_ptr_;

	//	std::string ctx_name_;

	//	std::chrono::milliseconds timeout_;
	//};

	enum class context_kind
	{
		stream,
		tcp_broadcast,
		transfer
	};

	template <auto Mode>
	class basic_context : public context_base
	{
	public:
		constexpr static auto mode = Mode;
	public:
		explicit basic_context(const std::string& name, std::chrono::steady_clock::duration timeout = 30ms)
			: context_base(name, timeout)
		{}

		template <typename... Args>
		void visit(Args&&...)
		{
			return;
		}
	};
} // namespace aquarius