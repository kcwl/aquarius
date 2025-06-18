#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/post.hpp>

namespace aquarius
{
	namespace context
	{
		namespace detail
		{
			template <typename Session>
			class handler_router
				: public single_router<handler_router<Session>, void, std::vector<char>, std::shared_ptr<Session>>
			{
			public:
				handler_router() = default;

			public:
				template <typename Request, typename Context>
				void regist(std::size_t proto)
				{
					auto func = [&](std::vector<char> buffer, std::shared_ptr<Session> session)
					{
						auto req = std::make_shared<Request>();

						req->unpack(buffer);

						post(session->get_executor(),
							 [=]
							 {
								 co_spawn(
									 session->get_executor(),
									 [session, req] -> awaitable<void>
									 {
										 auto ctx = std::make_shared<Context>();

										 co_await ctx->visit(req);

										 auto resp_buf = ctx->response().pack();

										 error_code ec{};

										 co_await session->async_send(resp_buf, ec);
									 },
									 detached);
							 });
					};

					this->map_invokes_[proto] = func;
				}
			};
		} // namespace detail
	} // namespace context
} // namespace aquarius