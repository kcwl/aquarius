// database.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <aquarius.hpp>
#include <iostream>
#include <serviced/srvd_client.hpp>
#include "mysql_config.hpp"

namespace po = boost::program_options;

using namespace aquarius::db;

int main(int argc, char* argv[])
{
	po::options_description cmd("database");

	std::string name("database");
	int32_t pool_size = static_cast<int32_t>(std::thread::hardware_concurrency());

	cmd.add_options()("help", "print help message");
	cmd.add_options()("listen", po::value<uint16_t>(), "listen port");
	cmd.add_options()("pool_size", po::value<int32_t>(&pool_size), "pool thread size");
	cmd.add_options()("name", po::value<std::string>(&name), "server name");
	cmd.add_options()("srvd_host", po::value<std::string>(), "serviced host");
	cmd.add_options()("srvd_port", po::value<uint16_t>(), "serviced port");
	cmd.add_options()("db_host", po::value<std::string>(), "database host");
	cmd.add_options()("db_port", po::value<uint16_t>(), "database port");
	cmd.add_options()("db_user", po::value<std::string>(), "database user");
	cmd.add_options()("db_passwd", po::value<std::string>(), "database password");
	cmd.add_options()("db_schema", po::value<std::string>(), "database schema");

	po::variables_map vm{};
	po::store(po::parse_command_line(argc, argv, cmd), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << cmd;
		return 0;
	}

	if (vm.count("listen"))
	{
		srv_config::get_mutable_instance().port = vm["listen"].as<uint16_t>();
	}
	else
	{
		std::cout << "the listen port is invalid\n";
		std::cout << cmd;
		return 0;
	}

	if (vm.count("pool_size"))
	{
		pool_size = vm["pool_size"].as<int32_t>();
	}

	if (vm.count("name"))
	{
		name = vm["name"].as<std::string>();
	}

	if (vm.count("srvd_host"))
	{
		srv_config::get_mutable_instance().srvd_host = vm["srvd_host"].as<std::string>();
	}
	else
	{
		std::cout << "the srvd_host is invalid\n";
		std::cout << cmd;
		return 0;
	}

	if (vm.count("srvd_port"))
	{
		srv_config::get_mutable_instance().srvd_port = vm["srvd_port"].as<uint16_t>();
	}
	else
	{
		std::cout << "the srvd_port is invalid\n";
		std::cout << cmd;
		return 0;
	}

	if (vm.count("db_host"))
	{
		mysql_config::get_mutable_instance().host = vm["db_host"].as<std::string>();
	}
	else
	{
		std::cout << "the db_host is invalid\n";
		std::cout << cmd;
		return 0;
	}

	if (vm.count("db_port"))
	{
		mysql_config::get_mutable_instance().port = vm["db_port"].as<uint16_t>();
	}
	else
	{
		std::cout << "the db_port is invalid\n";
		std::cout << cmd;
		return 0;
	}

	if (vm.count("db_user"))
	{
		mysql_config::get_mutable_instance().user = vm["db_user"].as<std::string>();
	}
	else
	{
		std::cout << "the db_user is invalid\n";
		std::cout << cmd;
		return 0;
	}

	if (vm.count("db_passwd"))
	{
		mysql_config::get_mutable_instance().password = vm["db_passwd"].as<std::string>();
	}
	else
	{
		std::cout << "the db_passwd is invalid\n";
		std::cout << cmd;
		return 0;
	}

	if (vm.count("db_schema"))
	{
		mysql_config::get_mutable_instance().db = vm["db_schema"].as<std::string>();
	}
	else
	{
		std::cout << "the db_schema is invalid\n";
		std::cout << cmd;
		return 0;
	}

	srv_config::get_mutable_instance().local_server_name = name;

	aquarius::tcp::server server(srv_config::get_mutable_instance().port, pool_size, name);

	server.run();
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
