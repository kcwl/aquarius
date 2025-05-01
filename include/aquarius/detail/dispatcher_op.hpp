#pragma once
#include <boost/asio/detail/operation.hpp>
#include <boost/asio/detail/handler_work.hpp>

namespace aquarius
{

    template<typename ConstBufferSequnce, typename Handler, typename PeerExecutor>
    class dispatcher_op : public boost::asio::detail::operation
    {
        using base_type = boost::asio::detail::operation;
    public:
        BOOST_ASIO_DEFINE_HANDLER_PTR(dispatcher_op);

    public:
        dispatcher_op(const boost::system::error_code& ec, Handler& handler, const ConstBufferSequnce& buffer, const PeerExecutor& peer_ex)
            : base_type(&dispatcher_op::do_complete)
            , handler_(static_cast<Handler&&>(handler))
            , work_(handler_, peer_ex)
        {

        }

    public:
        static void do_complete(void* owner, boost::asio::detail::operation* base, const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            auto op = static_cast<dispatcher_op*>(base);

            ptr p = { boost::asio::detail::addressof(op->handler_), op, 0 };

            // if(owner)
            //     op->do_assign();

            boost::asio::detail::handler_work<Handler, PeerExecutor> work(static_cast<boost::asio::detail::handler_work<Handler, PeerExecutor>&&>(op->work_));

            boost::asio::detail::binder1<Handler, boost::system::error_code> handler(0, static_cast<Handler&&>(op->handler_), op->ec_);

            p.h = boost::asio::detail::addressof(handler.handler_);
            p.reset();

            if (owner)
            {
                boost::asio::detail::fenced_block b(boost::asio::detail::fenced_block::half);
                BOOST_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_, "..."));
                work.complete(handler, handler.handler_);
                BOOST_ASIO_HANDLER_INVOCATION_END;
            }

            //auto method = find_method(proto);

            //if (!method)
            //    return;
            //flex_buffer_t write_buffer;
            //method->visit(request, write_buffer);
        }

        static void do_immediate(boost::asio::detail::operation* base, bool is_continuation, const void* executor)
        {
            //auto op = static_cast<dispatcher_op*>(base);

            //ptr p = { boost::asio::detail::addressof(op->handler_), op, 0 };

            //// op->do_assign();

            //boost::asio::detail::immediate_handler_work<Handler, PeerExecutor> work(static_cast<boost::asio::detail::handler_work<Handler, PeerExecutor>&&>(op->work_));

            //boost::asio::detail::binder1<Handler, boost::system::error_code> handler(0, static_cast<Handler&&>(op->handler_), op->ec_);

            //p.h = boost::asio::detail::addressof(handler.handler_);
            //p.reset();

            //BOOST_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_, "..."));
            //work.complete(handler, handler.handler_, executor);
            //BOOST_ASIO_HANDLER_INVOCATION_END;


            auto request = find_request(proto);

            if (!request)
            {
                return;
            }

            request->from_binary(message_buffer);


        }

    public:
        boost::system::error_code ec_;

    private:
        Handler handler_;

        boost::asio::detail::handler_work<Handler, PeerExecutor> work_;
    };
}