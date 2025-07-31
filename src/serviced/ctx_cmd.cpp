#include "client_factory.h"
#include "cmd_store.h"
#include "error.hpp"
#include <aquarius.hpp>
#include <proto/cmd.proto.hpp>
#include <ranges>

namespace serviced
{
	AQUARIUS_CONTEXT(cmd_opt, rpc_cmd)
	{
		auto input = message()->body().input;

		std::vector<std::string> cmds{};

		for (auto c : std::views::split(input, ' '))
		{
			cmds.push_back(std::string(std::string_view(c)));
		}

		if (cmds.empty())
			co_return errc::invalid_cmd;

		auto cmd_str = *cmds.begin();

		auto cmd_ptr = CMD.find(cmd_str);

		if (!cmd_ptr)
			co_return errc::invalid_cmd;

		if(!cmd_ptr->cmd_ptr)
			co_return errc::invalid_cmd;

		cmd_ptr->cmd_ptr->load_options(cmds);

		aquarius::error_code ec;

		try
		{
			ec = co_await cmd_ptr->opt_func(response().body().output);
		}
		catch (std::exception& ec)
		{
			XLOG_ERROR() << ec.what();
		}

		response().header()->result(ec.value());

		co_return errc::success;
	}
} // namespace serviced
