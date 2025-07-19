#pragma once
#include <memory>
#include <aquarius/session/session_store.hpp>
#include <aquarius/basic_sql_stream.hpp>

namespace aquarius
{
	namespace context
	{
		template <typename Session, typename Request, typename Response, typename E>
		class basic_handler
		{
			using context_t = boost::asio::execution::context_t;
		public:
			basic_handler(const std::string& name)
				: name_(name)
			{}

			virtual ~basic_handler() = default;

		public:
			auto visit(std::shared_ptr<Session> sessoin_ptr, std::shared_ptr<Request> message) -> awaitable<E>
			{
				session_ptr_ = sessoin_ptr;

				request_ = message;

				co_return co_await this->handle();
			}

			std::shared_ptr<Request> request() const
			{
				return request_;
			}

			Response& response()
			{
				return response_;
			}
			
			void set_rpc_id(std::size_t rpc_id)
			{
				rpc_id_ = rpc_id;
			}

			std::size_t rpc_id() const
			{
				return rpc_id_;
			}

			template<typename T>
			auto defer(std::size_t id)
			{
				return session::defer<Session, T>(id);
			}

			auto sql_engine()
			{
				return basic_sql_stream<>(static_cast<aquarius::io::io_context&>(session_ptr_->get_executor().query(context_t{})));
			}

		protected:
			virtual auto handle() -> awaitable<E> = 0;

		protected:
			std::shared_ptr<Request> request_;

			Response response_;

			std::string name_;

			std::size_t rpc_id_;

			std::shared_ptr<Session> session_ptr_;


		};

		template <typename Session, typename Request, typename Response, typename E>
		class stream_handler : public basic_handler<Session, Request, Response, E>
		{
			using base_type = basic_handler<Session, Request, Response, E>;

		public:
			stream_handler(const std::string& name)
				: base_type(name)
			{}

		public:
			auto visit(std::shared_ptr<Session> session_ptr, std::shared_ptr<Request> request) -> awaitable<E>
			{
				auto result = co_await base_type::visit(session_ptr, request);

				make_response(result);

				co_return result;
			}

		private:
			void make_response(E result)
			{
				// this->response().header()->set_result(static_cast<int64_t>(result));
			}
		};

		template <typename Protocol, typename RPC, typename Context>
		struct auto_handler_register
		{
			explicit auto_handler_register(std::size_t proto)
			{
				context::handler_router<Protocol>::get_mutable_instance()
					.template regist<typename RPC::request, Context>(proto);
			}
		};

		template <typename Protocol, typename Context>
		struct auto_transfer_handler_register
		{
			explicit auto_transfer_handler_register(std::size_t proto)
			{
				context::handler_router<Protocol>::get_mutable_instance().template regist<Context>(proto);
			}
		};
	} // namespace context
} // namespace aquarius

#define AQUARIUS_GLOBAL_ID(request) std::hash<std::string>()(#request)

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define AQUARIUS_STREAM_HANDLER(__session, method, error, __rpc)                                                                  \
	class method : public aquarius::context::stream_handler<__session, __rpc::request, __rpc::response, error>                    \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::context::stream_handler<__session, __rpc::request, __rpc::response, error>;                   \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##method))                                                    \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<error> override;                                                  \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<error>