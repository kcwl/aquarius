// client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <aquarius.hpp>
#include <proto/test_transfer.proto.hpp>

int main(int argc, char* argv[])
{
    aquarius::cmd_options cmd("client");

    cmd.add_option<std::string>("host", "server host");
    cmd.add_option<std::string>("port", "server port");

    cmd.load_options(argc, argv);

    aquarius::io_context io{};

    aquarius::tcp_client client(io, 3 ,100s);

    auto host = cmd.option<std::string>("host");
    auto port = cmd.option<std::string>("port");

    auto future = aquarius::co_spawn(io, [&]()->aquarius::awaitable<void> 
        {
            auto is_connect = co_await client.async_connect(host, port);

            if (!is_connect)
            {
                std::cout << "connect error\n";
                co_return;
            }

            auto msg_ptr = std::make_shared<rpc_test_transfer::request>();

            msg_ptr->topic(1);
            msg_ptr->header()->version(12);
            msg_ptr->body().test_message = "test transfer";
            

            client.async_send<rpc_test_transfer>(msg_ptr);
        },aquarius::use_future);

    std::thread t([&]() {return io.run(); });

    future.get();

    t.join();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
