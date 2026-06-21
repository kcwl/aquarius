#include <aquarius.hpp>
#include <iostream>
#include <srvd_client.hpp>


int main(int argc, char* argv[])
{
	srv_config::get_mutable_instance().host = "127.0.0.1";
	srv_config::get_mutable_instance().port = 3399;

	aquarius::cmd_options cmd("gateway");

	cmd.add_option<std::string>("help", "print help message");
	cmd.add_option<uint16_t>("listen", "listen port");
	cmd.add_option<int32_t>("pool_size", "pool thread size");
	cmd.add_option<std::string>("name", "server name");
	cmd.add_option<std::string>("transfer", "transfer ip addr");

	cmd.load_options(argc, argv);

	aquarius::tcp::server server(cmd.option<uint16_t>("listen"), cmd.option<int32_t>("pool_size"),
							 cmd.option<std::string>("name"));

	server.run();

	return 0;
}
