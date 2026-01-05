#include "server.hpp"
#include "transfer_client.h"
#include <aquarius/cmd_options.hpp>
#include <aquarius/logger.hpp>
#include <iostream>
#include "player_manager.h"

int main(int argc, char* argv[])
{
	aquarius::cmd_options cmd("gateway");

	cmd.add_option<std::string>("help", "print help message");
	cmd.add_option<uint16_t>("listen", "listen port");
	cmd.add_option<int32_t>("pool_size", "pool thread size");
	cmd.add_option<std::string>("name", "server name");
	cmd.add_option<std::string>("transfer", "transfer ip addr");

	TRANSFER.set_addr(cmd.option<std::string>("transfer"));

	cmd.load_options(argc, argv);

	gateway::server srv(cmd.option<uint16_t>("listen"), cmd.option<int32_t>("pool_size"),
						cmd.option<std::string>("name"));

	srv.set_accept_func([&] (std::shared_ptr<gateway::server::session_type> session_ptr) 
						{
							aquarius::insert_player(session_ptr->uuid());
						});

	srv.set

	srv.run();

	return 0;
}
