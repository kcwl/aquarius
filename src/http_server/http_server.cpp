// http_server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <aquarius.hpp>
#include <boost/describe.hpp>

struct test_login_req
{
	std::string user;
	std::string passwd;
};
struct test_login_resp
{
}; 

struct rpc_test_login
{
	constexpr static auto id = "/login";
	using request = aquarius::http_request<test_login_req>;
	using response = aquarius::http_response<test_login_resp>;
};


BOOST_DESCRIBE_STRUCT(test_login_req, (), (user, passwd))



AQUARIUS_HTTP_CONTEXT(ctx_login, rpc_test_login)
{
	response().result(aquarius::http_status::ok);

	co_return aquarius::error_code{};
}

int main()
{
    aquarius::http_server srv(80, 10, "test http server");

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
