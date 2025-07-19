#include "cmd_register.h"
#include "cmd_store.h"

namespace serviced
{
	void cmd_register()
	{
		auto help_op = std::make_shared<aquarius::cmd_options >("help");
		help_op->add_option<std::string>("-h", "help message");

		CMD.insert(help_op);

		auto  list_op = std::make_shared<aquarius::cmd_options>("list");
		help_op->add_option<std::string>("-h", "show server infos");

		CMD.insert(list_op);

		auto  add_op = std::make_shared<aquarius::cmd_options>("add");
		add_op->add_option<std::string>("--ip_addr", "server addr");
		add_op->add_option<std::string>("--port", "server port");

		CMD.insert(add_op);

		auto remove_op = std::make_shared<aquarius::cmd_options>("remove");
		remove_op->add_option<std::string>("--ip_addr", "server addr");
		remove_op->add_option<std::string>("--port", "server port");

		CMD.insert(remove_op);
	}
} // namespace serviced