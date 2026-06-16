#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/ip/handler_error.hpp>

namespace aquarius
{
	namespace serviced
	{
		template <typename Handler, typename Protocol>
		class basic_subscribe_context : public basic_context<Handler, Protocol>
		{
		public:
			using base_type = basic_context<Handler, Protocol>;

			using session_callback = typename base_type::session_callback;

		public:
			static auto do_complete(base_type* ctx, Protocol* proto, const session_callback& cb) -> asio::awaitable<error_code>
			{
				basic_subscribe_context* context = static_cast<basic_subscribe_context*>(ctx);

				if (!context)
				{
					return handle_error::not_exist;
				}

				context->handle_request(cb, )
			}

		private:
			void handle_request(const session_callback& cb)
			{
				if (!handler_ptr_)
				{
					handler_ptr_ = std::make_shared<Handler>();
				}

				handler_ptr_->attach_session(cb);

				auto ec = co_await handler_ptr->handle();

				if (ec)
				{
					co_return ec;
				}

				flex_buffer resp_buffer{};

				handler_ptr->response().commit(resp_buffer);

				co_return co_await cb(resp_buffer);
			}

		private:
			std::shared_ptr<Handler> handler_ptr_;
		};
	} // namespace serviced
} // namespace aquarius