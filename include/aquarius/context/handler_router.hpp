#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/post.hpp>
#include <vector>

namespace aquarius
{
	namespace context
	{
		template <typename Protocol>
		class handler_router : public single_router<handler_router<Protocol>, bool, std::shared_ptr<typename Protocol::session>, std::vector<char>,
													 typename Protocol::header>
		{
		public:
			using session = typename Protocol::session;

			using header = typename Protocol::header;

			using body_buffer = std::vector<char>;

		public:
			handler_router() = default;

		public:
			template <typename Request, typename Context>
			void regist(std::size_t proto)
			{
				auto func = [&](std::shared_ptr<session> session, body_buffer&& buffer,  header h)
				{
					auto req = std::make_shared<Request>(h);
					req->unpack(buffer);
					post(session->get_executor(),
						 [=]
						 {
							 co_spawn(
								 session->get_executor(),
								 [session, req]() mutable -> awaitable<void>
								 {
									 auto ctx = std::make_shared<Context>();

									 auto ec = co_await ctx->visit(req);

									 if (!ec)
									 {
										 std::vector<char> body_buff{};
										 ctx->response().pack(body_buff);
										 ctx->response().header()->length(body_buff.size());
										 ctx->response().header()->rpc_id(req->header()->rpc_id());
										 std::vector<char> header_buff{};
										 ctx->response().header()->pack(header_buff);
										 header_buff.insert(header_buff.end(), body_buff.begin(), body_buff.end());

										 co_await session->async_send(std::move(header_buff));
									 }
								 },
								 detached);
						 });
					return true;
				};
				this->map_invokes_[proto] = func;
			}
		};
	} // namespace context
} // namespace aquarius