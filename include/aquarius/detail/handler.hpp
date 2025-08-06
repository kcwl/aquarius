#pragma once
#include <aquarius/basic_sql_stream.hpp>
#include <aquarius/detail/handler_router.hpp>
#include <aquarius/detail/session_store.hpp>
#include <memory>

namespace aquarius
{
	namespace detail
	{
		template <typename Session, typename Message>
		class basic_handler
		{
		public:
			using message_type = Message;

		public:
			basic_handler(const std::string& name)
				: name_(name)
			{}

			virtual ~basic_handler() = default;

		public:
			auto visit(std::shared_ptr<Session> sessoin_ptr, std::shared_ptr<message_type> message)
				-> awaitable<error_code>
			{
				session_ptr_ = sessoin_ptr;

				message_ = message;

				co_return co_await this->handle();
			}

			std::shared_ptr<message_type> message() const
			{
				return message_;
			}

			std::string name() const
			{
				return name_;
			}

		protected:
			virtual auto handle() -> awaitable<error_code> = 0;

			auto session()
			{
				return session_ptr_;
			}

		private:
			std::string name_;

			std::shared_ptr<message_type> message_;

			std::shared_ptr<Session> session_ptr_;
		};

		template <typename Session, typename Request, typename Response>
		class basic_positive_handler : public basic_handler<Session, Request>
		{
			using base_type = basic_handler<Session, Request>;

			using context_t = boost::asio::execution::context_t;

		public:
			basic_positive_handler(const std::string& name)
				: base_type(name)
			{}

		public:
			auto visit(std::shared_ptr<Session> session_ptr, std::shared_ptr<Request> request) -> awaitable<error_code>
			{
				auto result = co_await base_type::visit(session_ptr, request);

				make_response(result);

				co_return result;
			}

			Response& response()
			{
				return response_;
			}

		protected:
			template <typename T>
			auto defer(std::size_t id)
			{
				return defer<Session, T>(id);
			}

			auto sql_engine()
			{
				basic_sql_stream<>(static_cast<io_context&>(this->session()->get_executor().query(context_t{})));
			}

			void make_response(error_code /*result*/)
			{
				// this->response().header()->set_result(static_cast<int64_t>(result));
			}

		private:
			Response response_;
		};

		template <typename Protocol, typename RPC, typename Context>
		struct auto_handler_register
		{
			explicit auto_handler_register(std::string_view proto)
			{
				handler_router<Protocol>::get_mutable_instance().template regist<RPC, Context>(proto);
			}
		};
	} // namespace detail
} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define AQUARIUS_HANDLER(__session, method, __request)                                                                 \
	class method final : public aquarius::detail::basic_handler<__session, __request>                                  \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::detail::basic_handler<__session, __request>;                                       \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##method))                                                    \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<aquarius::error_code>

#define AQUARIUS_POSITIVE_HANDLER(__session, method, __request, __response)                                            \
	class method final : public aquarius::detail::basic_positive_handler<__session, __request, __response>             \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::detail::basic_positive_handler<__session, __request, __response>;                  \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##method))                                                    \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<aquarius::error_code>
