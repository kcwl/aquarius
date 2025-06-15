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
				: public single_router<handler_router<Session>, void, std::vector<char>&, std::shared_ptr<Session>>
			{
			public:
				handler_router() = default;

			public:
				template <typename RPC, typename Context>
				void regist(std::size_t proto)
				{
					auto func = [&](std::vector<char> buffer, std::shared_ptr<Session> session)
					{
						auto rpc = std::make_shared<RPC>();

						rpc->unpack_req(buffer);

						post(session->get_executor(),
							 [=]
							 {
								 co_spawn(
									 session->get_executor(),
									 [session, rpc] -> awaitable<void>
									 {
										 co_await std::make_shared<Context>()->visit(rpc);

										 auto resp_buf = rpc->pack_resp();

										 error_code ec{};

										 session->async_send(resp_buf, ec);
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