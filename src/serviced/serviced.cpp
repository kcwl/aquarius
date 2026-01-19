// serviced.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "server.h"
#include "cmd_register.h"
#include <aquarius/module/player_schedule.hpp>
#include "player.h"

using namespace aquarius::serviced;

int main()
{
    cmd_register();

    server srv(3399, 10, "serviced");

    srv.set_accept_func([&] (std::shared_ptr<server::session_type> session_ptr) ->aquarius::awaitable<void>
                        {
                            auto player_ptr = std::make_shared<player>(session_ptr->uuid());

                            co_await aquarius::mpc_player_insert("player_module"sv, session_ptr->uuid(), player_ptr);
                        });

    srv.set_close_func([&] (std::shared_ptr<server::session_type> session_ptr) ->aquarius::awaitable<void>
                       {
                           auto player_ptr = std::make_shared<player>(session_ptr->uuid());

                           co_await aquarius::mpc_player_erase<player>("player_module"sv, session_ptr->uuid());
                       });

    srv.run();
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
