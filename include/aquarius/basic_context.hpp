#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/ip/handler_error.hpp>

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
		using function_type = std::function<asio::awaitable<error_code>(basic_protocol_context*, Protocol*, Args&&...)>;

	public:
		basic_protocol_context(function_type func)
			: func_(func)
		{}

		virtual ~basic_protocol_context() = default;

	public:
		auto complete(Protocol* proto, Args&&... args) -> asio::awaitable<error_code>
		{
			co_return co_await func_(this, proto, std::forward<Args>(args)...);
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

	public:
		basic_context()
			: base_type(&basic_context<Handler, Protocol, Args...>::do_complete)
		{}

		virtual ~basic_context() = default;

	public:
		static auto do_complete(base_type* base, Protocol* proto, Args&&... args) -> asio::awaitable<error_code>
		{
			auto ptr = static_cast<basic_context*>(base);

			if (!ptr)
			{
				co_return handle_error::not_exist;
			}

			co_return co_await proto->template handle_request<Handler>(ptr->handler_ptr_, std::forward<Args>(args)...);
		}

		virtual error_code visit(flex_buffer& buffer) override
		{
			if (!handler_ptr_)
			{
				handler_ptr_ = std::make_shared<handler_type>();
			}

			return handler_ptr_->visit(buffer);
		}

	private:
		std::shared_ptr<handler_type> handler_ptr_;
	};
} // namespace aquarius