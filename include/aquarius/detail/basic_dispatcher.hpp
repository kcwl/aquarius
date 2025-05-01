#pragma once
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/awaitable.hpp>
#include <aquarius/detail/dispatch_service.hpp>

namespace aquarius
{
	template<typename Executor = boost::asio::any_io_executor>
	class basic_dispatcher
	{
	public:
		using executor_type = Executor;

	private:
        class initiate_async_send_message
        {
        public:
            using executor_type = Executor;

            explicit initiate_async_send_message(basic_dispatcher* self)
                : self_(self)
            {

            }

            const executor_type& get_executor() const noexcept
            {
                return self_->get_executor();
            }

        public:
            template<typename Handler, typename Executor1, typename ConstBufferSequence>
            void operator()(Handler&& handler, const Executor1& executor, const ConstBufferSequence& buffers)
            {
                boost::asio::detail::non_const_lvalue<Handler> handler2(handler);

                self_->impl_.get_service().async_send_message(handler2.value, buffers, executor);
            }

        private:
            basic_dispatcher* self_;
        };

	public:
        explicit basic_dispatcher(const executor_type& executor)
            : impl_(0, executor)
        {
        }

        template<typename ExecutorContext>
        explicit basic_dispatcher(ExecutorContext& context)
            : impl_(0, 0, context)
        {

        }

    public:
        const executor_type& get_executor() noexcept
        {
            return impl_.get_executor();
        }

        template<typename ConstBufferSequence, BOOST_ASIO_COMPLETION_TOKEN_FOR(void(boost::system:error_code, std::unique_ptr<handle_method_base>, std::unique_ptr<request>)) Token = boost::asio::default_completion_token_t<executor_type>>
        auto async_dispatch(const ConstBufferSequence& buffers, Token&& token = boost::asio::default_completion_token_t<executor_type>())
        {
            return boost::asio::async_initiate<Token, void(boost::system::error_code)>(initiate_async_send_message(this), token, impl_.get_executor(), buffers);
        }

    private:
        boost::asio::detail::io_object_impl<dispatcher_service, executor_type> impl_;
	};
}