#include <iostream>
#include <aquarius.hpp>
#include <login/proto/login.virgo.h>

using namespace aquarius;

int main()
{
    asio::io_context io{};

    auto cli = std::make_shared<tcp::client>(io, 30s);

    auto futuer = asio::co_spawn(io, [&] ()->asio::awaitable<void>
                   {
                       auto ec = co_await cli->async_connect("127.0.0.1", 8100);

                       if (ec.value() == 0)
                       {
                           std::cout << "connect success;\n";
                       }
                       else
                       {
                           std::cout << "connect failed!\n";
                       }
                   }, asio::use_future);

    std::thread t([&] () { io.run(); });

    auto status = futuer.wait_for(5s);
    if (status != std::future_status::ready)
    {
        std::cout << "connect timeout!\n";
        return 0;
    }

    while (true)
    {
        std::string input{};

        std::cin >> input;

        if (input == "quit")
        {
            break;
        }

        if (input == "login")
        {
            auto request = std::make_shared<login_tcp_request>();

            request->body().username() = "test";
            request->body().password() = "test";

            asio::co_spawn(io, [&, request] ()->asio::awaitable<void>
                           {
                               auto resp = co_await cli->async_call<login_tcp_response>(request);

                               if (resp.result() == 0)
                               {
                                   std::cout << "login success;\n";
                               }
                               else
                               {
                                   std::cout << "login failed!\n";
                               }
                           },asio::detached);
        }
    }
}