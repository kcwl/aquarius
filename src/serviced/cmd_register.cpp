#include "cmd_register.h"
#include "cmd_store.h"
#include "client_factory.h"

namespace serviced
{
	void cmd_register()
	{
		auto help_op = std::make_shared<cmd_info>("help");
		help_op->cmd_ptr->add_option<std::string>("h", "help message");
		help_op->opt_func = [](std::string& output)
			{
				auto cmds = CMD.cmds();

				for (auto& c : cmds)
				{
					if (!c.second)
						continue;

					output.append(c.second->cmd_ptr->desc() + "\t\t");

					for (const auto& o : c.second->cmd_ptr->options())
					{
						if (!o)
							continue;

						output.append(o->description());
					}

					output.append("\n");
				}
			};

		CMD.insert(help_op);

		auto  list_op = std::make_shared<cmd_info>("list");
		help_op->cmd_ptr->add_option<std::string>("h", "show server infos");
		help_op->opt_func = [](std::string& output)
			{
				auto clients = CLIENT.get_client();

				for (auto& c : clients)
				{
					output.append(c->remote_address() + ":" + std::to_string(c->remote_port()) + "\n");
				}
			};

		CMD.insert(list_op);

		auto  add_op = std::make_shared<cmd_info>("add");
		add_op->cmd_ptr->add_option<std::string>("ip_addr", "server addr");
		add_op->cmd_ptr->add_option<std::string>("port", "server port");

		add_op->opt_func = [&](std::string& output)
			{
				auto client_ptr = std::make_shared<client_factory::client>();

				auto ip_addr = add_op->cmd_ptr->option<std::string>("ip_addr");

				client_ptr->async_connect(ip_addr, add_op->cmd_ptr->option<std::string>("--port"));

				CLIENT.insert(ip_addr, client_ptr);
			};

		CMD.insert(add_op);

		auto remove_op = std::make_shared<cmd_info>("remove");
		remove_op->cmd_ptr->add_option<std::string>("ip_addr", "server addr");
		remove_op->cmd_ptr->add_option<std::string>("port", "server port");

		remove_op->opt_func = [&](std::string& output)
			{
				auto ip_addr = remove_op->cmd_ptr->option<std::string>("ip_addr");

				CLIENT.remove(ip_addr);
			};

		CMD.insert(remove_op);
	}
} // namespace serviced