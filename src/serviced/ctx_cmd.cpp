#include "cmd_store.h"
#include <aquarius.hpp>
#include <proto/cmd.proto.hpp>
#include "error.hpp"
#include "client_factory.h"

namespace serviced
{
	AQUARIUS_STREAM_CONTEXT(cmd_help, rpc_cmd_help)
	{
		auto cmds = CMD.cmds();

		for (auto& c : cmds)
		{
			if (!c.second)
				continue;

			this->response().body().msg.append(c.second->desc()+"\t\t");

			for (const auto& o : c.second->options())
			{
				if (!o)
					continue;

				this->response().body().msg.append(o->description());
			}


			this->response().body().msg.append("\n");
		}

		co_return errc::success;
	}

	AQUARIUS_STREAM_CONTEXT(cmd_list, rpc_cmd_list)
	{
		auto clients = CLIENT.get_client();

		for (auto& c : clients)
		{
			response().body().msg.append(c->remote_address() + ":" + std::to_string(c->remote_port()) + "\n");
		}

		co_return errc::success;
	}

	AQUARIUS_STREAM_CONTEXT(cmd_add, rpc_cmd_add)
	{
		auto client_ptr = std::make_shared<client_factory::client>();

		auto cmd = CMD.find("add");

		if (!cmd)
			co_return errc::invalid_cmd;

		(*cmd).load_options(request()->body().input);

		auto ip_addr = (*cmd).option<std::string>("--ip_addr");

		client_ptr->async_connect(ip_addr, (*cmd).option<std::string>("--port"));

		CLIENT.insert(ip_addr, client_ptr);

		co_return errc::success;
	}

	AQUARIUS_STREAM_CONTEXT(cmd_remove, rpc_cmd_remove)
	{
		auto cmd = CMD.find("add");

		if (!cmd)
			co_return errc::invalid_cmd;

		(*cmd).load_options(request()->body().input);

		auto ip_addr = (*cmd).option<std::string>("--ip_addr");

		CLIENT.remove(ip_addr);

		co_return errc::success;
	}
} // namespace serviced
