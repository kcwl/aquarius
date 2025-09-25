#pragma once
#include <aquarius/basic_router.hpp>

namespace aquarius
{
	template <typename Session>
	class tcp_router : public basic_router<Session>,  public  singleton<tcp_router<Session>>
	{
		using base = basic_router<Session>;

		using typename base::buffer_t;

	public:
		tcp_router() = default;

		virtual ~tcp_router() = default;

	public:
		template <typename Context>
		void regist(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, buffer_t&& buffer)
			{
				auto req = std::make_shared<typename Context::request_t>();

				req->consume(buffer);

				post(session->get_executor(),
					 [=]
					 {
						 co_spawn(
							 session->get_executor(), [session, req]() mutable -> awaitable<void>
							 { co_await std::make_shared<Context>()->visit(session, req); }, detached);
					 });
				return true;
			};
			this->map_invokes_->add(proto, func);
		}
		template <typename... Args>
		bool invoke(std::string_view key, Args... args)
		{
			auto func = this->map_invokes_->find(key);
			return func == nullptr ? false : func(args...);
		}
	};
} // namespace aquarius