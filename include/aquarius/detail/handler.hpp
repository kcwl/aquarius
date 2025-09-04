#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/detail/sql_stream.hpp>
#include <aquarius/detail/router.hpp>
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
			auto visit(std::shared_ptr<Session> session_ptr, std::shared_ptr<Request> request) -> awaitable<void>
			{
				auto result = co_await base_type::visit(session_ptr, request);

				co_await make_response(result);
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
				sql_stream<>(static_cast<io_context&>(this->session()->get_executor().query(context_t{})));
			}

			virtual auto make_response(error_code) -> awaitable<void> = 0;

		private:
			Response response_;
		};

		template <typename Session, typename Request, typename Response>
		class tcp_handler : public basic_positive_handler<Session, Request, Response>
		{
		public:
			tcp_handler(const std::string& name)
				: basic_positive_handler<Session, Request, Response>(name)
			{}

		public:
			virtual auto make_response(error_code ec) -> awaitable<void> override
			{
				detail::flex_buffer<char> body_buff{};
				auto result = this->response().commit(body_buff);

				if (!result.has_value())
				{
					co_return;
				}

				this->response().length(body_buff.active());
				this->response().rpc(this->message()->rpc());
				detail::flex_buffer<char> header_buff{};
				result = this->response().header().commit(header_buff);

				if (!result.has_value())
					co_return;

				header_buff.append(std::move(body_buff));

				co_await this->session()->async_send(std::move(header_buff));
			}
		};

		template <typename Session, typename Request, typename Response>
		class http_handler : public basic_positive_handler<Session, Request, Response>
		{
		public:
			http_handler()
				: basic_positive_handler<Session, Request, Response>(name)
			{}

		public:
			virtual void make_response(error_code ec) override
			{
				this->response().base_header().version(this->message()->base_header().version());
				this->response().base_header().result(ec.value);
				this->response().base_header().reason(ec.what());
				std::vector<char> header_buff{};
				this->response().base_header().commit(header_buff);
				header_buff.push_back('\r');
				header_buff.push_back('\n');

				this->response().commit(header_buff);

				co_await this->session()->async_send(std::move(header_buff));
			}
		};

		template <typename Protocol, typename RPC, typename Context>
		struct auto_handler_register
		{
			explicit auto_handler_register(const std::string& proto)
			{
				router<Protocol>::get_mutable_instance().template regist<RPC, Context>(proto);
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

#define __AQUARIUS_TCP_HANDLER(__session, method, __request, __response)                                               \
	class method final : public aquarius::detail::tcp_handler<__session, __request, __response>                        \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::detail::tcp_handler<__session, __request, __response>;                             \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##method))                                                    \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<aquarius::error_code>

#define __AQUARIUS_HTTP_HANDLER(__session, method, __request, __response)                                              \
	class method final : public aquarius::detail::http_handler<__session, __request, __response>                       \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::detail::http_handler<__session, __request, __response>;                            \
                                                                                                                       \
	public:                                                                                                            \
		method()                                                                                                       \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##method))                                                    \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::awaitable<aquarius::error_code> override;                                   \
	};                                                                                                                 \
	inline auto method::handle() -> aquarius::awaitable<aquarius::error_code>
