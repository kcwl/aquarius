#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/basic_router.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/post.hpp>
#include <type_traits>
#include <vector>

namespace aquarius
{
	namespace detail
	{
		template <typename Session>
		class handler_router : public single_router<handler_router<Session>, bool, std::shared_ptr<Session>,
													std::vector<char>, typename Session::header>
		{
		public:
			using session = Session;

			using header = typename Session::header;

			using body_buffer = std::vector<char>;

		public:
			handler_router() = default;

		public:
			template <typename RPC, typename Context>
			void regist_tcp(std::string_view proto)
			{
				auto func = [&](std::shared_ptr<session> session, body_buffer&& buffer, header h)
				{
					auto req = std::make_shared<typename RPC::request>(h);
					req->consume(buffer);
					post(session->get_executor(),
						 [=]
						 {
							 co_spawn(
								 session->get_executor(),
								 [session, req]() mutable -> awaitable<void>
								 {
									 auto ctx = std::make_shared<Context>();

									 auto ec = co_await ctx->visit(session, req);

									 if constexpr (session::is_server)
									 {
										 if (!ec)
										 {
											 std::vector<char> body_buff{};
											 ctx->response().commit(body_buff);
											 ctx->response().length(body_buff.size());
											 ctx->response().rpc(req->rpc());
											 std::vector<char> header_buff{};
											 ctx->response().header().commit(header_buff);
											 header_buff.insert(header_buff.end(), body_buff.begin(), body_buff.end());

											 co_await session->async_send(std::move(header_buff));
										 }
									 }
								 },
								 detached);
						 });
					return true;
				};
				this->map_invokes_->add(proto, func);
			}

			template <typename RPC, typename Context>
			void regist_http(std::string_view proto)
			{
				auto func = [&](std::shared_ptr<session> session, body_buffer&& buffer, header h)
					{
						auto req = std::make_shared<typename RPC::request>(h);
						req->consume(buffer);
						post(session->get_executor(),
							[=]
							{
								co_spawn(
									session->get_executor(),
									[session, req]() mutable -> awaitable<void>
									{
										auto ctx = std::make_shared<Context>();

										auto ec = co_await ctx->visit(session, req);

										if constexpr (session::is_server)
										{
											if (!ec)
											{
												//std::vector<char> body_buff{};
												//ctx->response().commit(body_buff);
												//ctx->response().header()->length(body_buff.size());

												//std::vector<char> header_buff{};
												//ctx->response().commit
												//ctx->response().header().commit(header_buff);
												//header_buff.push_back('\r');
												//header_buff.push_back('\n');

												//header_buff.insert(header_buff.end(), body_buff.begin(), body_buff.end());

												std::vector<char> header_buff{};
												ctx->response().base_header().commit(header_buff);
												header_buff.push_back('\r');
												header_buff.push_back('\n');

												co_await session->async_send(std::move(header_buff));
											}
										}
									},
									detached);
							});
						return true;
					};
				this->map_invokes_->add(proto, func);
			}
		};
	} // namespace detail
} // namespace aquarius