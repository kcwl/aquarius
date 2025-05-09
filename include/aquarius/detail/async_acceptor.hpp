#pragma once
#include <aquarius/detail/io_concepts.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/noncopyable.hpp>

namespace aquarius
{
    template <typename Protocol, typename Executor>
    class async_acceptor : public boost::noncopyable
    {
    public:
        using acceptor_type = boost::asio::basic_socket_acceptor<Protocol, Executor>;

        using protocol_type = typename acceptor_type::protocol_type;

        using endpoint_type = typename acceptor_type::endpoint_type;

        using executor_type = typename acceptor_type::executor_type;

    public:
        template <execution_context_convertible ExecutionContext>
        async_acceptor(ExecutionContext& context, uint16_t port, bool reuse_addr = true)
            : impl_(context, endpoint_type{ protocol_type::v4(), port }, reuse_addr)
        {
        }

        async_acceptor(async_acceptor&&) = default;

        async_acceptor& operator=(async_acceptor&&) = default;

        virtual ~async_acceptor() = default;

    public:
        auto accept(boost::system::error_code& ec) -> boost::asio::awaitable<typename protocol_type::socket>
        {
            auto sock = co_await impl_.async_accept(boost::asio::redirect_error(boost::asio::use_awaitable, ec));

            if (!impl_.is_open())
            {
                ec = boost::asio::error::bad_descriptor;
            }

            co_return sock;
        }

        const executor_type& get_executor() noexcept
        {
            return impl_.get_executor();
        }

        void close(boost::system::error_code& ec)
        {
            impl_.cancel(ec);

            impl_.close(ec);
        }

        void close()
        {
            impl_.cancel();

            impl_.close();
        }

    private:
        acceptor_type impl_;
    };
} // namespace aquarius