#pragma once
#include <aquarius/basic_router.hpp>

namespace aquarius
{
	template <typename Session>
	class tcp_router
		: public basic_router<Session, std::function<bool(std::shared_ptr<Session>, detail::flex_buffer<char>&)>>,
		  public singleton<tcp_router<Session>>
	{
		using base = basic_router<Session, std::function<bool(std::shared_ptr<Session>, detail::flex_buffer<char>&)>>;

		using buffer_t = detail::flex_buffer<char>;

	public:
		tcp_router() = default;

		virtual ~tcp_router() = default;

	public:
		template <typename Context>
		void regist(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, buffer_t& buffer)
			{
				auto req = std::make_shared<typename Context::request_t>();

				error_code ec{};

				req->consume(buffer, ec);

				if (ec)
				{
					return false;
				}

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
		bool invoke(std::string_view key, Args&&... args)
		{
			bool result = false;

			auto func = this->map_invokes_->find(key);

			if (!func)
				return result;

			try
			{
				result = func(std::forward<Args>(args)...);
			}
			catch (std::exception& ex)
			{
				XLOG_ERROR() << "[" << key << "] invoke error! error: " << ex.what();
			}

			return result;
		}
	};
} // namespace aquarius