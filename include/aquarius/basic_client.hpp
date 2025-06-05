#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/logger.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>

namespace aquarius
{
    template <typename Protocol, typename Session>
    class basic_client : public std::enable_shared_from_this<basic_client<Protocol, Session>>
    {
        using socket_type = boost::asio::basic_stream_socket<Protocol>;

        using endpoint_type = boost::asio::ip::basic_endpoint<Protocol>;

        using resolve_type = boost::asio::ip::basic_resolver<Protocol>;

        using session_type = Session;

    public:
        basic_client(int reconnect_times = 3, std::chrono::steady_clock::duration timeout = 30ms)
            : io_service_()
            , session_ptr_(nullptr)
            , timer_(io_service_)
            , reconnect_times_(reconnect_times)
            , timeout_(timeout)
            , ip_addr_()
            , port_()
        {

        }

        explicit basic_client(const std::string& ip_addr, const std::string& port, int reconnect_times = 3, std::chrono::steady_clock::duration timeout = 30ms)
            : basic_client(reconnect_times, timeout)
        {
            async_connect(ip_addr, port);
        }

    public:
        void run()
        {
            io_service_.run();
        }

        void stop()
        {
            io_service_.stop();

            session_ptr_->shutdown();
        }

        void close()
        {
            session_ptr_->close();
        }

        bool async_connect(const std::string& ip_addr, const std::string& port)
        {
            ip_addr_ = ip_addr;
            port_ = port;

            error_code ec{};

            socket_type socket(io_service_);

            auto future = boost::asio::co_spawn(io_service_, [&]->boost::asio::awaitable<void>
                {
                    auto addr = boost::asio::ip::make_address(ip_addr.c_str(), ec);

                    if (ec)
                    {
                        co_return;
                    }

                    endpoint_type endpoint(addr, static_cast<boost::asio::ip::port_type>(std::atoi(port.c_str())));

                    co_await socket->async_connect(endpoint, boost::asio::redirect_error(boost::asio::use_awaitable, ec));

                }, boost::asio::use_future);

            if (!future.get() || ec)
                return false;

            if (!create_session(std::move(socket)))
            {
                return false;
            }

            boost::asio::co_spawn(io_service_, session_ptr_->start(), boost::asio::detached);

            return true;
        }

        template <typename RPC, typename Request = typename RPC::tcp_request, typename Response = RPC::tcp_response>
        std::optional<Response> async_send(const Request& req)
        {
            flex_buffer fs{};
            req.to_binary(fs);

            std::promise<std::promise<Response>> resp_promise{};
            auto future = resp_promise.get_future();

             client_invoke::regist(random_session_id(), [promise = std::move(resp_promise)](std::optional<Response> resp)
             {
                 promise.set_value(resp);
             });

            boost::asio::co_spawn(io_service_, session_ptr_->send_packet(Request::proto, std::move(fs)), boost::asio::detached);

            return future.get();
        }

        template<typename RPC, typename Func, typename Request = typename RPC::tcp_request, typename Response = typename RPC::tcp_response>
        void async_send(Request&& req, Func&& f)
        {
            flex_buffer fs{};
            req.to_binary(fs);

            client_invoke::regist<Response>(random_session_id(), std::forward<Func>(f));

            boost::asio::co_spawn(io_service_, session_ptr_->send_packet(Request::proto, std::move(fs)), boost::asio::detached);
        }

        std::string remote_address()
        {
            return session_ptr_->remote_address();
        }

        uint32_t remote_address_u()
        {
            return session_ptr_->remote_address_u();
        }

        int remote_port()
        {
            return session_ptr_->remote_port();
        }

        template<typename Func>
        void async_wait(std::chrono::system_clock::duration dura, Func&& f)
        {
            boost::system::error_code ec;
            timer_.expires_after(dura);

            timer_.async_wait([&](const boost::system::error_code& ec)
                {
                    if (ec)
                        return;

                    if (timer_.expiry() <= std::chrono::steady_clock::now())
                    {
                        f();
                        return;
                    }

                    async_wait(dura, std::forward<Func>(f));
                });
        }

        template<typename Func>
        void set_close_func(Func&& f)
        {
            close_func_ = std::forward<Func>(f);
        }

    private:
        std::size_t random_session_id()
        {
            return 0;
        }

        bool create_session(socket_type socket)
        {
            session_ptr_ = std::make_shared<session_type>(std::move(socket));

            session_ptr_->regist_close(close_func_);

            session_ptr_->set_reconnect_func([&, reconnect_times = this->reconnect_times_] mutable
                {
                    while (reconnect_times--)
                    {
                        if (async_connect(ip_addr_, port_))
                            break;
                    }

                    if (reconnect_times == 0)
                    {

                    }
                });

            return true;
        }

    private:
        boost::asio::io_context io_service_;

        std::shared_ptr<Session> session_ptr_;

        boost::asio::steady_timer timer_;

        std::function<void()> close_func_;

        int reconnect_times_;

        std::chrono::steady_clock::duration timeout_;

        std::string ip_addr_;

        std::string port_;
    };
} // namespace aquarius