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
		std::string_view input = message()->body().input;

		std::vector<std::string> cmds{};

		for (auto c : std::views::split(input, " "))
		{
			cmds.push_back(c.data());
		}

		if (cmds.empty())
			co_return errc::invalid_cmd;

		auto cmd_str = *cmds.begin();

		auto cmd_ptr = CMD.find(cmd_str);

		if (!cmd_ptr)
			co_return errc::invalid_cmd;

		cmd_ptr->opt_func(response().body().output);

		co_return errc::success;
	}
} // namespace serviced
