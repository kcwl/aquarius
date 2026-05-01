#include <aquarius.hpp>
#include <aquarius/http.hpp>
#include <aquarius/tcp.hpp>
#include <iostream>

namespace aquarius
{
	HTTP_CONFIG_INVOKE(cfg)
	{}
} // namespace aquarius

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

	srv.run();

	return 0;
}
