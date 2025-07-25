// serviced-ssh.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <aquarius/cmd_options.hpp>
#include <cmd.proto.hpp>
#include "client.h"
#include <ranges>

int main(int argc, char* argv[])
{
    aquarius::cmd_options cmd("serviced");

    cmd.add_option<std::string>("host", "serviced ip addr");
    cmd.add_option<std::string>("port", "serviced port");

    cmd.load_options(argc, argv);

    auto ip_addr = cmd.option<std::string>("host");

    auto port = cmd.option<std::string>("port");

    if (ip_addr.empty())
    {
        std::cout << "host:" << ip_addr << "is not invalid!" << std::endl;
        return 0;
    }

    if (port.empty())
    {
        std::cout << "host:" << port << "is not invalid!" << std::endl;
        return 0;
    }

    auto future = CLIENT.async_connect(ip_addr, port);

    auto result = future.get();

    if (!result)
    {
        return 0;
    }

    std::string input{};

    std::cout << "serviced-ssh> ";

    while (true)
    {
        rpc_cmd::request req{};

        std::cin >> req.body().input;

        if (req.body().input.empty())
            continue;

        if (req.body().input == "quit" || req.body().input == "q")
            break;

        CLIENT.async_send<rpc_cmd>(std::move(req));
    }

    return 0;
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
