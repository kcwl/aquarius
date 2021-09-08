// aquarius.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <locale>
//#include "aquarius/detail/streambuf.hpp"
//#include "aquarius/detail/router.hpp"
//#include "aquarius/request.hpp"
//#include "aquarius/async_control.hpp"
#include "test.hpp"
#include "aquarius.hpp"

//class local_client : public aquarius::aqnet::aquarius_client
//{
//public:
//	local_client(boost::asio::io_service& io_service, const boost::asio::ip::tcp::resolver::results_type& endpoints)
//		: aquarius::aqnet::aquarius_client(io_service, endpoints) {}
//
//	template<class... Args>
//	local_client(boost::asio::io_service& io_service, Args&&... args)
//		: aquarius::aqnet::aquarius_client(io_service, args...) {}
//public:
//	virtual int handler() { return 1; }
//};

//static aquarius::detail::ContextRegister<A> a(std::size_t(10001));



int main()
{
	//aquarius::server aq_server{ "9100" ,4};
	//aq_server.run();

	aquarius::streambuf buf;
	buf << 10001 << 2 << 3 << 4 << 5;

	aquarius::context ctx;
	ctx.process(nullptr, buf);

	//handle_ptr->process();

	//int i = 0;
	//message_stream ms{1024};
	////auto iter = ms.begin();
	////*iter = std::byte(i++);
	////iter++;
	////*iter = std::byte(i++);
	////iter++;
	////*iter = std::byte(i++);
	////iter++;
	////*iter = std::byte(i++);

	//ms.push(1, 2, 3);

	//auto s = ms.begin();

	//auto e = ms.end();

	////std::cout << static_cast<int>(*iter) << std::endl;

	//for(auto iter : ms)
	//{
	//	std::cout << static_cast<int>(iter) << std::endl;
	//}

	//std::string str{"hello world"};
	//for(auto it =str.begin();it != str.end();it++)
	//{
	//	std::cout << *it << std::endl;
	//}



	//aquarius::aqcore::aq_buffer buffer_;

	//int n = 5;
	//buffer_.write(n);

	//int m = 0;

	//std::cout << buffer_.read<int>() << std::endl;

	//std::cout << buffer_.get_read_pointer() << std::endl;
	//aquarius::aqcore::aq_message_queue<std::pair<int,std::string>> queue_;

	//boost::asio::io_service io_service;
	//local_client client{ io_service,"127.0.0.1","9100" };
	std::cin.get();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
