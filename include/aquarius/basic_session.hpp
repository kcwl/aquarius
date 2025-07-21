#pragma once
#include <aquarius/config.hpp>
#include <aquarius/any_io_executor.hpp>
#include <aquarius/awaitable.hpp>
#include <aquarius/concepts.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/session/session_object_impl.hpp>
#include <aquarius/session/session_service.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/flex_buffer.hpp>
#include <span>

#ifdef AQUARIUS_ENABLE_SSL
#include <aquarius/detail/ssl_session_service.hpp>
#endif

namespace aquarius
{
    template <typename Protocol>
    class basic_session : public std::enable_shared_from_this<basic_session<Protocol>>
    {
    public:
#ifdef AQUARIUS_ENABLE_SSL
        using socket_type = typename detail::ssl_session_service<Protocol>::ssl_socket_type;
        using impl_type = detail::session_object_impl<detail::ssl_session_service<Protocol>>;
#else
        using socket_type = typename session::session_service<Protocol>::socket_type;
        using impl_type = session::session_object_impl<session::session_service<Protocol>>;
#endif

        using acceptor = typename Protocol::acceptor;

        constexpr static auto is_server = Protocol::is_server;

    public:
        explicit basic_session(socket_type socket)
            : impl_(std::move(socket))
        {
        }

        virtual ~basic_session() = default;

    public:
        auto get_executor()
        {
            return impl_.get_executor();
        }

        std::size_t id() const
        {
            return id_;
        }

        std::string remote_address() const
        {
            return impl_.get_service().remote_address(impl_.get_implementation());
        }

        uint32_t remote_address_u() const
        {
            return impl_.get_service().remote_address_u(impl_.get_implementation());
        }

        uint16_t remote_port() const
        {
            return impl_.get_service().remote_port(impl_.get_implementation());
        }

        auto protocol() -> awaitable<error_code>
        {
            co_await impl_.get_service().start(impl_.get_implementation());

            error_code ec;

            for (;;)
            {
                co_await proto_.recv(this->shared_from_this(), ec);

                if (ec)
                {
                    if (ec != boost::asio::error::eof)
                    {
                        XLOG_ERROR() << "on read some occur error - " << ec.what();
                    }

                    shutdown();

                    co_return ec;
                }
            }

            std::unreachable();
        }

        template <typename BufferSequence>
        auto async_read(BufferSequence& buff) -> awaitable<error_code>
        {
            error_code ec;

            co_await impl_.get_service().async_read(impl_.get_implementation(), buff, ec);

            co_return ec;
        }

        template <typename BufferSequence>
        auto async_send(BufferSequence buffer) -> awaitable<void>
        {
            error_code ec{};
            co_await impl_.get_service().async_write_some(impl_.get_implementation(), std::move(buffer), ec);

            if (ec)
            {
                XLOG_ERROR() << "async write is failed! maybe " << ec.message();
            }
        }

        void shutdown()
        {
            impl_.get_service().shutdown();
        }

        void close()
        {
            impl_.get_service().close(impl_.get_implementation());
        }

    private:
        impl_type impl_;

        Protocol proto_;

        std::size_t id_;
    };
} // namespace aquarius