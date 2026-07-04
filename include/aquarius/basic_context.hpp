#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/handler_error.hpp>
#include <aquarius/logger.hpp>

namespace aquarius
{
	class context_base
	{
	public:
		virtual ~context_base() = default;
	};

	template <typename Protocol, typename... Args>
	class basic_protocol_context : public context_base
	{
	public:
		using session_callback = typename Protocol::session_callback;

		using function_type = std::function<asio::awaitable<error_code>(
			basic_protocol_context*, Protocol*, flex_buffer&, const session_callback&, Args...)>;

	public:
		basic_protocol_context(function_type func)
			: func_(func)
		{}

		virtual ~basic_protocol_context() = default;

	public:
		auto complete(Protocol* proto, flex_buffer& buffer, const session_callback& cb, Args&&... args)
			-> asio::awaitable<error_code>
		{
			co_return co_await func_(this, proto, buffer, cb, std::forward<Args>(args)...);
		}

		void attach_router(const std::string& router)
		{
			router_ = router;
		}

		std::string router() const
		{
			return router_;
		}

		virtual error_code visit(flex_buffer&)
		{
			XLOG_WARNING() << "the context[" << router_ << "] does not have handler";
			return error_code{};
		}

	private:
		function_type func_;

		std::string router_;
	};

	template <typename Handler, typename Protocol, typename... Args>
	class basic_context : public basic_protocol_context<Protocol, Args...>
	{
	public:
		using base_type = basic_protocol_context<Protocol, Args...>;

		using handler_type = Handler;

		using session_callback = typename base_type::session_callback;

	public:
		basic_context()
			: base_type(&basic_context<Handler, Protocol, Args...>::do_complete)
			, handler_ptr_(std::make_shared<handler_type>())
		{}

		virtual ~basic_context() = default;

	public:
		static auto do_complete(base_type* base, Protocol* proto, flex_buffer& buffer, const session_callback& cb, Args&&... args)
			-> asio::awaitable<error_code>
		{
			auto ptr = static_cast<basic_context*>(base);

			if (!ptr)
			{
				co_return handle_error::not_exist;
			}

			ptr->handler_ptr_->visit(buffer);

			asio::co_spawn(co_await asio::this_coro::executor,
						   proto->template handle_request<Handler>(ptr->handler_ptr_, cb, std::forward<Args>(args)...),
						   asio::detached);
		}

	private:
		std::shared_ptr<handler_type> handler_ptr_;
	};
} // namespace aquarius