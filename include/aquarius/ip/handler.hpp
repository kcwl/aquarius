#pragma once
#include <aquarius/concepts.hpp>
#include <aquarius/module/handler_channel.hpp>
#include <expected>

namespace aquarius
{
	template <typename Request, typename Response>
	class handler
	{
	public:
		using request_t = Request;
		using response_t = Response;

	public:
		handler(const std::string& name)
			: request_ptr_(new Request)
			, name_(name)
		{}

	public:
		virtual auto visit(flex_buffer& buffer, int method, error_code& ec) -> asio::awaitable<flex_buffer>
		{
			request()->method(method);

			using type = decltype(request()->body());

			if constexpr (has_set_method<type>)
			{
				request()->body().set_method(method);
			}

			request()->consume(buffer);

			make_response(co_await this->handle());

			flex_buffer resp_buffer{};

			ec = response().commit(resp_buffer);

			co_return std::move(resp_buffer);
		}

		Response& response()
		{
			return response_;
		}

		std::shared_ptr<request_t> request() const
		{
			return request_ptr_;
		}

		std::string name() const
		{
			return name_;
		}

		void session(std::size_t uid)
		{
			session_id_ = uid;
		}

		std::size_t session() const
		{
			return session_id_;
		}

	protected:
		virtual auto handle() -> asio::awaitable<error_code> = 0;

	private:
		void make_response(error_code result)
		{
			response().result(result.value());
		}

	private:
		std::shared_ptr<request_t> request_ptr_;

		std::string name_;

		response_t response_;

		std::size_t session_id_;
	};

	template <typename Handler>
	struct auto_handler_register
	{
		explicit auto_handler_register(std::string_view proto)
		{
			auto f = [](flex_buffer& buffer, std::size_t session_id, int method) -> asio::awaitable<std::expected<flex_buffer, error_code>>
			{
				auto handler_ptr = std::make_shared<Handler>();

				handler_ptr->session(session_id);

				error_code ec{};

				auto resp_buffer = co_await handler_ptr->visit(buffer, method, ec);

				if (ec)
				{
					co_return std::unexpected(ec);
				}

				co_return resp_buffer;
			};
			mpc_subscribe(std::string(proto), f);
		}
	};

} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request

#define __AQUARIUS_HANDLER_IMPL(__handler, __request, __response)                                                      \
	class __handler final : public aquarius::handler<__request, __response>                                            \
	{                                                                                                                  \
	public:                                                                                                            \
		using base_type = aquarius::handler<__request, __response>;                                                    \
                                                                                                                       \
	public:                                                                                                            \
		__handler()                                                                                                    \
			: base_type(AQUARIUS_GLOBAL_STR_ID(__handler_##__handler))                                                 \
		{}                                                                                                             \
		virtual auto handle() -> aquarius::asio::awaitable<aquarius::error_code> override;                             \
	};                                                                                                                 \
	inline auto __handler::handle() -> aquarius::asio::awaitable<aquarius::error_code>

#define AQUARIUS_HANDLER(__request, __response, __handler)                                                             \
	class __handler;                                                                                                   \
	[[maybe_unused]] static aquarius::auto_handler_register<__handler> __auto_register_##__handler(                    \
		__request::this_router);                                                                                       \
	__AQUARIUS_HANDLER_IMPL(__handler, __request, __response)