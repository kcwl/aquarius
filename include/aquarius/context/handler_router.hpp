#pragma once
#include <aquarius/basic_router.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/ip/lowyer_header.hpp>
#include <aquarius/post.hpp>

namespace aquarius
{
	namespace context
	{
		namespace detail
		{
			template <typename Session>
			class handler_router
				: public single_router<handler_router<Session>, void, std::vector<char>, std::shared_ptr<Session>, local_header>
			{
			public:
				handler_router() = default;

			public:
				template <typename Request, typename Context>
				void regist(std::size_t proto)
				{
					auto func = [&](std::vector<char> buffer, std::shared_ptr<Session> session, local_header header)
					{
						auto req = std::make_shared<Request>();

						req->unpack(buffer);

						post(session->get_executor(),
							 [=]
							 {
								 co_spawn(
									 session->get_executor(),
									 [session, req, resp_header = std::move(header)] mutable -> awaitable<void>
									 {
										 auto ctx = std::make_shared<Context>();

										 co_await ctx->visit(req);

										 std::vector<char> resp_buf(sizeof(local_header));

										 ctx->response().pack(resp_buf);

										 resp_header.length = resp_buf.size() - sizeof(local_header);

										 std::memcpy(resp_buf.data(), (char*)&resp_header, sizeof(local_header));

										 error_code ec{};

										 co_await session->async_send(resp_buf, ec);
									 },
									 detached);
							 });
					};

					this->map_invokes_[proto] = func;
				}

				template<typename TransferContext>
				void regist(std::size_t proto)
				{
					auto func = [&](std::vector<char> buffer, std::shared_ptr<Session> session, local_header header)
						{
							post(session->get_executor(),
								[=, buffer = std::move(buffer)]
								{
									co_spawn(
										session->get_executor(),
										[session, resp_header = std::move(header), buffer = std::move(buffer)] mutable -> awaitable<void>
										{
											auto ctx = std::make_shared<TransferContext>();

											std::memcpy(ctx->request()->data(), (char*)&resp_header, sizeof(local_header));

											auto buff_ptr = std::make_shared<std::vector<char>>();

											ctx->set_rpc_id(resp_header.rpc_id);

											std::copy((char*)&resp_header, (char*)&resp_header + sizeof(local_header), std::back_inserter(*buff_ptr));

											std::copy(buffer.begin(), buffer.end(), std::back_inserter(*buff_ptr));

											co_await ctx->visit(buff_ptr);

											error_code ec{};

											co_await session->async_send(ctx->response(), ec);
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