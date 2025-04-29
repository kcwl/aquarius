#pragma once
#include <boost/asio/detail/io_object_impl.hpp>

#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/detail/ssl_context.hpp>
#endif

namespace aquarius
{
    template<typename Service, typename Executor = boost::asio::any_io_executor>
    class session_object_impl : public boost::asio::detail::io_object_impl<Service, Executor>
    {
    public:
        using base_type = boost::asio::detail::io_object_impl<Service, Executor>;

        using socket_type = typename Service::socket_type;

    public:
        session_object_impl(boost::asio::execution_context& context)
            : base_type(0, 0, context)
        {

        }

        session_object_impl(socket_type socket)
            : base_type(0, socket.get_executor())
        {
            this->get_service().move_copy(this->get_implementation(), std::move(socket));
        }
    };

#ifdef AQUARIUS_ENABLE_SSL
    template<typename Service, typename Executor = boost::asio::any_io_executor>
    class ssl_session_object_impl : public boost::asio::detail::io_object_impl<Service, Executor>
    {
    public:
        using base_type = boost::asio::detail::io_object_impl<Service, Executor>;

        using socket_type = typename Service::socket_type;

        using ssl_context_type = typename Service::ssl_context_type;

    public:
        ssl_session_object_impl(socket_type socket, ssl_context_type& context = ssl_context<>::create())
            : base_type(0, socket.get_executor())
        {
            this->get_service().move_copy(this->get_implementation(), std::move(socket), context);
        }
    };
#endif

} // namespace aquarius