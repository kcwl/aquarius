#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/trie.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/post.hpp>
#include <aquarius/singleton.hpp>
#include <type_traits>
#include <vector>

namespace aquarius
{
	namespace detail
	{
		template <typename Session>
		class handler_router : public singleton<handler_router<Session>>
		{
		public:
			using session = Session;

			using header = typename Session::header;

			using body_buffer = std::vector<char>;

			using function_type = std::function<bool(std::shared_ptr<Session>, body_buffer, typename Session::header)>;

			using func_trie = detail::trie<function_type>;

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
										 ctx->response().base_header().version(req->base_header().version());
										 ctx->response().base_header().result(ec.value);
										 ctx->response().base_header().reason(ec.what());
										 std::vector<char> header_buff{};
										 ctx->response().base_header().commit(header_buff);
										 header_buff.push_back('\r');
										 header_buff.push_back('\n');

										 ctx->response().commit(header_buff);

										 co_await session->async_send(std::move(header_buff));
									 }
								 },
								 detached);
						 });
					return true;
				};
				this->map_invokes_->add(proto, func);
			}

			template<typename... Args>
			bool invoke(std::string_view key, Args... args)
			{
				auto func = this->map_invokes_->find(key);

				return func == nullptr ? false : func(args...);
			}

		private:
			std::shared_ptr<func_trie> map_invokes_;
		};
	} // namespace detail
} // namespace aquarius