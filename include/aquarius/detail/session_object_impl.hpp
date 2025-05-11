#pragma once
#include <boost/asio/detail/io_object_impl.hpp>
#include <aquarius/detail/ssl_context_factory.hpp>

namespace aquarius
{
    template<typename Service, typename Executor, std::size_t SSLVersion>
    class session_object_impl : public boost::asio::detail::io_object_impl<Service, Executor>
    {
    public:
        using base_type = boost::asio::detail::io_object_impl<Service, Executor>;

        using socket_type = typename Service::socket_type;

        using ssl_context_type = typename Service::ssl_context_type;

    public:
        session_object_impl(socket_type socket)
            : base_type(0, socket.get_executor())
        {
            this->get_service().move_copy(this->get_implementation(), std::move(socket), ssl_context_factory<ssl_context_type, SSLVersion>::create());
        }
    };

} // namespace aquarius