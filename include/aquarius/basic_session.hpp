#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <span>
#include <aquarius/detail/ssl_context.hpp>
#include <boost/asio/read.hpp>
#include <aquarius/asio.hpp>
#include <boost/asio/connect.hpp>
#include <aquarius/timer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <aquarius/ip/tcp/tcp_error.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/socket_adaptor.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/ip/protocol.hpp>

namespace aquarius
{

    template <auto ProtoTag, typename Adaptor>
    class basic_session
    {
    public:
        using socket = typename protocol<ProtoTag>::socket;

        using endpoint = typename protocol<ProtoTag>::endpoint;

        using acceptor = typename protocol<ProtoTag>::acceptor;

        using resolver = typename protocol<ProtoTag>::resolver;

        using executor_type = typename socket::executor_type;

    public:
        explicit basic_session(socket sock)
            : socket_(std::move(sock))
            , socket_adaptor_(socket_)
            , uuid_(detail::uuid_generator()())
        {
        }

        virtual ~basic_session() = default;

    public:
        auto get_executor()
        {
            return socket_.get_executor();
        }

        std::size_t uuid() const
        {
            return uuid_;
        }

        auto async_connect(const std::string& host, const std::string& port) -> awaitable<bool>
        {
            auto ec = co_await socket_adaptor_.async_connect(host, port);

            if (ec)
            {
                XLOG_ERROR() << "async_connect error: " << ec.what();
            }

            co_return !ec;
        }

        std::string remote_address() const
        {
            return socket_.remote_endpoint().address().to_string();
        }

        uint32_t remote_address_u() const
        {
            return socket_.remote_endpoint().address().to_v4().to_uint();
        }

        uint16_t remote_port() const
        {
            return socket_.remote_endpoint().port();
        }

        auto async_read(flex_buffer& buffer, std::size_t length) -> awaitable<error_code>
        {
            error_code ec;

            auto mutable_buffer = buffer.prepare(length);

            co_await boost::asio::async_read(socket_adaptor_.get_implement(), buffer, redirect_error(use_awaitable, ec));

            if (ec)
            {
                XLOG_INFO() << "async_read exception: " << ec.what() << ", remote_address: " << remote_address();
            }

            buffer.commit(length);

            co_return ec;
        }

        auto async_read_util(flex_buffer& buffer, std::string_view delm) -> awaitable<error_code>
        {
            error_code ec;

            co_await boost::asio::async_read_until(socket_adaptor_.get_implement(), buffer, delm,
                                                   redirect_error(use_awaitable, ec));

            if (ec)
            {
                XLOG_INFO() << "async_read_util exception: " << ec.what() << ", remote_address: " << remote_address();
            }

            co_return ec;
        }

        auto async_send(const flex_buffer& buffer) -> awaitable<error_code>
        {
            error_code ec{};

            co_await socket_adaptor_.get_implement().async_write_some(buffer.data(), redirect_error(use_awaitable, ec));

            co_return ec;
        }

        void shutdown()
        {
            error_code ec;

            socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
        }

        void close()
        {
            error_code ec;

            socket_.close(ec);
        }

        bool keep_alive(bool value)
        {
            error_code ec;

            socket_.set_option(typename protocol<ProtoTag>::keep_alive(value), ec);

            return !ec;
        }

        bool set_nodelay(bool enable)
        {
            error_code ec;

            socket_.set_option(typename protocol<ProtoTag>::no_delay(enable), ec);

            return !ec;
        }

    protected:
        socket socket_;

        Adaptor socket_adaptor_;

        std::size_t uuid_;
    };
} // namespace aquarius