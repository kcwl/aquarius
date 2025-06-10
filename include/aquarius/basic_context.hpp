#pragma once
#include <aquarius/detail/context_base.hpp>
#include <aquarius/use_future.hpp>

namespace aquarius
{
	template <typename Protocol, auto Mode>
	class basic_context : public context_base
	{
	public:
		constexpr static auto mode = Mode;

	public:
		basic_context(const std::string& name, std::chrono::steady_clock::duration timeout)
			: context_base(name, timeout)
		{}

	public:
		template <typename... Args>
		void visit(Args&&...)
		{
			return;
		}

		template <typename Session, typename Func>
		void invoke(std::shared_ptr<Session> session, Func&& f)
		{
			auto future =
				co_spawn(session->get_executor(), [func = std::move(f)] -> awaitable<void> { func(); }, use_future);

			auto status = future.wait_for(this->timeout());
			if (status == std::future_status::timeout)
			{
				//
			}
		}
	};
} // namespace aquarius