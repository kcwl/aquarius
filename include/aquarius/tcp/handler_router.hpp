#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/post.hpp>

namespace aquarius
{
	namespace tcp
	{
		template <typename Session>
		class handler_router
			: public single_router<handler_router<Session>, void, flex_buffer, std::shared_ptr<Session>>
		{
		public:
			handler_router() = default;

		public:
			template <typename Request, typename Context>
			void regist(std::size_t proto)
			{
				auto func = [&](flex_buffer buffer, std::shared_ptr<Session> session)
				{
					auto request = std::make_shared<Request>();

					request->from_binary(buffer);

					post(session->get_executor(),
						 [=]
						 {
							 co_spawn(
								 session->get_executor(),
								 [=] -> awaitable<void>
								 {
									 auto resp = co_await std::make_shared<Context>()->visit(request);

									 flex_buffer resp_buf{};
									 resp.to_binary(resp_buf);

									 session->async_send(proto, resp_buf);
								 },
								 detached);
						 });
				};

				this->map_invokes_[proto] = func;
			}
		};
	} // namespace tcp
} // namespace aquarius