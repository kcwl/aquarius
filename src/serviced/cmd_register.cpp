#include "cmd_register.h"
#include "client_factory.h"
#include "cmd_store.h"
#include "error.hpp"
#include <aquarius/async.hpp>

namespace serviced
{
	void cmd_register()
	{
		auto help_op = std::make_shared<cmd_info>("help");
		help_op->cmd_ptr->add_option<std::string>("h", "help message");
		help_op->opt_func = [](std::string& output) -> aquarius::awaitable<aquarius::error_code>
		{
			auto cmds = CMD.cmds();

			for (auto& c : cmds)
			{
				if (!c.second)
					continue;

				output.append(c.second->cmd_ptr->desc() + ":\n");

				for (const auto& o : c.second->cmd_ptr->options())
				{
					if (!o)
						continue;

					output.append("\t--" + o->long_name() + "\t" + o->description() + "\n");
				}
			}

			co_return errc::success;
		};

		CMD.insert(help_op);

		auto list_op = std::make_shared<cmd_info>("list");
		list_op->cmd_ptr->add_option<std::string>("h", "show server infos");
		list_op->opt_func = [](std::string& output) -> aquarius::awaitable<aquarius::error_code>
		{
			auto clients = CLIENT.get_client();

			for (auto& c : clients)
			{
				output.append(c->remote_address() + ":" + std::to_string(c->remote_port()) + "\n");
			}

			co_return errc::success;
		};

		CMD.insert(list_op);

		auto add_op = std::make_shared<cmd_info>("add");
		add_op->cmd_ptr->add_option<std::string>("host", "server addr");
		add_op->cmd_ptr->add_option<std::string>("port", "server port");
		add_op->cmd_ptr->add_option<std::size_t>("topic", "channel topic");

		add_op->opt_func = [add_op](std::string& output) -> aquarius::awaitable<aquarius::error_code>
		{
			auto ip_addr = add_op->cmd_ptr->option<std::string>("host");

			auto port = add_op->cmd_ptr->option<std::string>("port");

			auto client_ptr = aquarius::async_generator<client_factory::client>(ip_addr, port);

			CLIENT.insert(add_op->cmd_ptr->option<std::size_t>("topic"), ip_addr, port, client_ptr);

			co_return errc::success;
		};

		CMD.insert(add_op);

		auto remove_op = std::make_shared<cmd_info>("remove");
		remove_op->cmd_ptr->add_option<std::string>("host", "server addr");
		remove_op->cmd_ptr->add_option<std::string>("port", "server port");
		remove_op->cmd_ptr->add_option<std::size_t>("topic", "server channel topic");

		remove_op->opt_func = [remove_op](std::string& output) -> aquarius::awaitable<aquarius::error_code>
		{
			auto ip_addr = remove_op->cmd_ptr->option<std::string>("host");
			auto port = remove_op->cmd_ptr->option<std::string>("port");
			auto topic = remove_op->cmd_ptr->option<std::size_t>("topic");

			CLIENT.remove(topic, ip_addr, port);

			co_return errc::success;
		};

		CMD.insert(remove_op);
	}
} // namespace serviced