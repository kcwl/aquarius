// serviced-ssh.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <aquarius/cmd_options.hpp>
#include <cmd.proto.hpp>
#include "client.h"

void help()
{
    rpc_cmd_help::request req{};
    CLIENT.async_send<rpc_cmd_help>(req);
}

void list()
{
    rpc_cmd_list::request req{};
    CLIENT.async_send<rpc_cmd_list>(req);
}

void add(const std::string& input)
{
    rpc_cmd_add::request req{};
    req.body().input = input;

    CLIENT.async_send<rpc_cmd_add>(req);
}

void remove(const std::string& input)
{
    rpc_cmd_add::request req{};
    req.body().input = input;

    CLIENT.async_send<rpc_cmd_add>(req);
}

int main(int argc, char* argv[])
{
    aquarius::cmd_options cmd{};

    cmd.add_option<std::string>("--ip_addr", "serviced ip addr");
    cmd.add_option<std::string>("--port", "serviced port");

    cmd.load_options(argc, argv);

    auto ip_addr = cmd.option<std::string>("--ip_addr");

    auto port = cmd.option<std::string>("--port");

    if (ip_addr.empty())
    {
        std::cout << "ip addr:" << ip_addr << "is not invalid!" << std::endl;
        return 0;
    }

    if (port.empty())
    {
        std::cout << "ip addr:" << port << "is not invalid!" << std::endl;
        return 0;
    }

    auto future = CLIENT.async_connect(ip_addr, port);

    auto result = future.get();

    if (!result)
    {
        return 0;
    }

    std::string input{};

    while (true)
    {
        std::cin >> input;

        if (input == "quit" || input == "q")
            break;

        auto pos = input.find_first_of(" ");
        if (pos == std::string::npos)
        {
            XLOG_ERROR() << "command error!";
            continue;
        }

        auto key = input.substr(0, pos);

        if (key == "help")
        {
            help();
        }
        else if (key == "list")
        {
            list();
        }
        else if (key == "add")
        {
            add(input);
        }
        else if (key == "remove")
        {
            remove(input);
        }
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
