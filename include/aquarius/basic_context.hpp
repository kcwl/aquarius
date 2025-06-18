#pragma once
#include <aquarius/detail/context_base.hpp>
#include <aquarius/use_future.hpp>

namespace aquarius
{
	enum class mode
	{
		stream,
		transfer
	};

	template <typename Session>
	class basic_context : public context_base
	{
		using buffer_type = typename Session::buffer_type;

	public:
		basic_context(const std::string& name, std::chrono::steady_clock::duration timeout)
			: context_base(name, timeout)
		{}

	public:
		virtual void visit(buffer_type buff, std::shared_ptr<Session> session)
		{
			return;
		}

		template <typename Func>
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

	template <typename Handler>
	struct basic_transfer_context
	{
		template<typename Session>
		static auto invoke(std::vector<char>, std::shared_ptr<Session>)
		{
			return;
		}
	};

} // namespace aquarius