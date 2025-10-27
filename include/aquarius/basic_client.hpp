#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_context.hpp>
#include <aquarius/logger.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <boost/asio/connect.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/coroutine.hpp>

namespace aquarius
{
    using namespace std::chrono_literals;

    template <typename Session>
    class basic_client : public std::enable_shared_from_this<basic_client<Session>>
    {
        using socket = Session::socket;

        using resolver = Session::resolver;

    public:
        explicit basic_client(io_context& context, const std::string& host, const std::string& port, std::chrono::steady_clock::duration timeout = 1s)
            : io_context_(context)
            , host_(host)
            , port_(port)
            , session_ptr_(nullptr)
            , close_func_()
            , timeout_(timeout)
        {

        }

        basic_client(io_context& context, std::chrono::steady_clock::duration timeout = 1s)
            : basic_client(context, {}, {}, timeout)
        {

        }

        virtual ~basic_client() = default;

    public:
        auto async_connect() -> awaitable<error_code>
        {
            co_return co_await async_connect(host_, port_);
        }

        auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code>
        {
            host_ = host;
            port_ = port;

            session_ptr_ = std::make_shared<Session>(io_context_);

            co_return co_await session_ptr_->async_connect(host_, port_);
        }

        auto reconnect() -> awaitable<error_code>
        {
            co_return co_await async_connect();
        }

        template <typename Response, typename Request>
        auto query(std::shared_ptr<Request> req) -> awaitable<Response>
        {
            try
            {
                flex_buffer buffer{};

                session_ptr_->make_request_buffer(req, buffer);

                auto ec = co_await session_ptr_->async_send(std::move(buffer));

                if (ec)
                {
                    if (ec != boost::asio::error::eof)
                    {
                        XLOG_ERROR() << "on read some occur error - " << ec.what();
                    }
                    session_ptr_->shutdown();
                    if (close_func_)
                        close_func_();

                    co_return Response{};
                }

                co_return co_await session_ptr_->template query<Response>();
            }
            catch (error_code& ec)
            {
                XLOG_ERROR() << "make_request_buffer error - " << ec.what();
            }
        }

        std::string remote_address() const
        {
            return session_ptr_->remote_address();
        }

        int remote_port() const
        {
            return session_ptr_->remote_port();
        }

        template <typename Func>
        void set_close_func(Func&& f)
        {
            close_func_ = std::forward<Func>(f);
        }

        void close()
        {
            session_ptr_->close();

            std::shared_ptr<Session>().swap(session_ptr_);
        }

    private:
        io_context& io_context_;

        std::string host_;

        std::string port_;

        std::shared_ptr<Session> session_ptr_;

        std::function<void()> close_func_;

        std::chrono::steady_clock::duration timeout_;
    };
} // namespace aquarius