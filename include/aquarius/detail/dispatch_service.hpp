#pragma once
#include <aquarius/detail/dispatcher_op.hpp>

namespace aquarius
{
    class dispatcher_service : public boost::asio::detail::execution_context_service_base<dispatcher_service>
    {
        using base_type = boost::asio::detail::execution_context_service_base<dispatcher_service>;

    public:
        struct implementation_type {};

    public:
        dispatcher_service(boost::asio::execution_context& ctx)
            : base_type(ctx)
        {
        }

        template<typename ConstBufferSequence>
        std::size_t send_message(const ConstBufferSequence& buffers, boost::system::error_code& ec)
        {
            return buffers.size();
        }

        template<typename ConstBufferSequence, typename Handler, typename PeerExecutor>
        void async_send_message(Handler& handler, const ConstBufferSequence& buffer, const PeerExecutor& peer_ex)
        {
            bool is_continuation = boost_asio_handler_cont_helpers::is_continuation(handler);

            using op = dispatcher_op<ConstBufferSequence, Handler, PeerExecutor>;

            typename op::ptr p = { boost::asio::detail::addressof(handler), op::ptr::allocate(handler), 0 };

            p.p = new (p.v) op(success_ec_, handler, buffer, peer_ex);

            start_op(p.p, is_continuation, &peer_ex);

            p.p = p.v = nullptr;
        }

        void construct(implementation_type&) {}

        void destroy(implementation_type&) {}

        virtual void shutdown() override
        {
            return;
        }

    private:
        template<typename Op>
        void start_op(Op* op, bool is_continuation, const void* immediate_arg)
        {
            do_start_op(op, &Op::do_immediate, is_continuation, immediate_arg);
        }

        template<typename Op>
        void do_start_op(Op* op, void(*on_immediate)(boost::asio::detail::operation* op, bool, const void*), bool is_continuation, const void* immediate_arg)
        {
            on_immediate(op, is_continuation, immediate_arg);
        }

    private:
        const boost::system::error_code success_ec_;
    };
}