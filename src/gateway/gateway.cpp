#include "server.hpp"
#include "transfer_module.h"
#include <aquarius/cmd_options.hpp>
#include <aquarius/ip/http/http_client.hpp>
#include <aquarius/logger.hpp>
#include <iostream>
#include "player.h"

int main(int argc, char* argv[])
{
	aquarius::cmd_options cmd("gateway");

	cmd.add_option<std::string>("help", "print help message");
	cmd.add_option<uint16_t>("listen", "listen port");
	cmd.add_option<int32_t>("pool_size", "pool thread size");
	cmd.add_option<std::string>("name", "server name");
	cmd.add_option<std::string>("transfer", "transfer ip addr");

	cmd.load_options(argc, argv);

	aquarius::gateway::server srv(cmd.option<uint16_t>("listen"), cmd.option<int32_t>("pool_size"),
								  cmd.option<std::string>("name"));

	srv.set_accept_func(
		[&](std::shared_ptr<aquarius::gateway::server::session_type> session_ptr) -> aquarius::awaitable<void>
		{ co_await aquarius::mpc_player_insert(session_ptr->uuid(), std::make_shared<aquarius::gateway::player>(session_ptr->uuid())); });

	srv.set_close_func(
		[&](std::shared_ptr<aquarius::gateway::server::session_type> session_ptr) -> aquarius::awaitable<void>
		{ co_await aquarius::mpc_player_erase<aquarius::gateway::server::session_type>(session_ptr->uuid()); });

	srv.run();

	return 0;
}
