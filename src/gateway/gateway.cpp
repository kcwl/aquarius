#include <aquarius.hpp>
#include <iostream>

namespace aquarius
{
	HTTP_CONFIG_INVOKE(cfg)
	{

	}

	MYSQL_CONFIG_INVOKE(cfg)
	{

	}
}


int main(int argc, char* argv[])
{
	aquarius::cmd_options cmd("gateway");

	cmd.add_option<std::string>("help", "print help message");
	cmd.add_option<uint16_t>("listen", "listen port");
	cmd.add_option<int32_t>("pool_size", "pool thread size");
	cmd.add_option<std::string>("name", "server name");
	cmd.add_option<std::string>("transfer", "transfer ip addr");

	cmd.load_options(argc, argv);

	aquarius::tcp_server srv(cmd.option<uint16_t>("listen"), cmd.option<int32_t>("pool_size"),
								  cmd.option<std::string>("name"));

	std::thread t([&] { srv.run(); });

	aquarius::http_server http_srv(cmd.option<uint16_t>("listen"), cmd.option<int32_t>("pool_size"),
							 cmd.option<std::string>("name"));

	std::thread t1([&] { http_srv.run(); });

	t.join();
	t1.join();

	return 0;
}
