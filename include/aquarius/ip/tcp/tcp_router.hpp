#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/singleton.hpp>

namespace aquarius
{
	template <typename Session>
	class tcp_router : public basic_router<Session, int, std::function<bool(std::shared_ptr<Session>, flex_buffer&)>>,
					   public singleton<tcp_router<Session>>
	{
		using base = basic_router<Session, int, std::function<bool(std::shared_ptr<Session>, flex_buffer&)>>;

		constexpr static int router_key = 0;

	public:
		tcp_router() = default;

		virtual ~tcp_router() = default;

	public:
		template <typename Context>
		void regist(std::string_view proto)
		{
			auto func = [&](std::shared_ptr<Session> session, flex_buffer& buffer)
			{
				try
				{
					auto req = std::make_shared<typename Context::request_t>();

					req->consume(buffer);

					post(session->get_executor(),
						 [=]
						 {
							 co_spawn(
								 session->get_executor(),
								 [session, req, this]() mutable -> awaitable<void>
								 {
									 auto ec = co_await std::make_shared<Context>()->visit(req);

									 if (ec)
									 {

									 }
								 },
								 detached);
						 });
				}
				catch (error_code& ec)
				{
					co_spawn(session->get_executor(),
							 [&ec, session, this]() mutable -> awaitable<void>
							 {
								 typename Context::response_t resp{};
								 resp.result(ec.value());
								 co_await send_response(session, resp);
							 });

					return false;
				}

				return true;
			};

			this->push(router_key, proto, func);
		}

		template <typename... Args>
		bool invoke(std::string_view key, Args&&... args)
		{
			auto iter = this->map_invokes_.find(router_key);
			if (iter == this->map_invokes_.end())
				return false;

			auto tree = iter->second;
			if (tree == nullptr)
				return false;

			auto func = tree->find(key);

			return func == nullptr ? false : func(std::forward<Args>(args)...);
		}

	private:
		template<typename Session, typename Response>
		auto send_response(std::shared_ptr<Session> session_ptr, Response& resp) -> awaitable<void>
		{
			if (!session_ptr)
				co_return;

			flex_buffer buf{};
			constexpr auto pos = sizeof(uint32_t);

			buf.commit(pos);

			resp.commit(buf);

			auto len = static_cast<uint32_t>(buf.size() - pos);

			buf.pubseekpos(0, std::ios::beg);
			buf.sputn((char*)&len, pos);

			co_await session_ptr->async_send(buf);
		}
	};
} // namespace aquarius