// dbadmin.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <aquarius/cmd_options.hpp>
#include "server.h"

int main(int argc, char* argv[])
{
    aquarius::cmd_options cmd{"dbamind"};
    cmd.add_option<uint16_t>("listen", "listen port");
    cmd.add_option<std::string>("host", "db ip address");
    cmd.add_option<std::string>("user", "db user");
    cmd.add_option<std::string>("passwd", "db user password");
    cmd.add_option<std::string>("name", "server name");
    cmd.add_option<int32_t>("pool_size", "server thread size");

    cmd.load_options(argc, argv);

    db::server srv(cmd.option<uint16_t>("listen"), cmd.option<int32_t>("pool_size"),
        cmd.option<std::string>("name"), cmd.option<std::string>("host"), cmd.option<std::string>("user"), cmd.option<std::string>("passwd"));

    srv.run();

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
