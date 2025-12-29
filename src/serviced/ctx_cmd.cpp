#include "cmd_store.h"
#include "error.hpp"
#include "proto/cmd.virgo.h"
#include <aquarius.hpp>
#include <ranges>

namespace serviced
{
	AQUARIUS_HANDLER(cmd_op_tcp_request, cmd_op_tcp_response, ctx_cmd)
	{
		auto input = request()->body().input;

		std::vector<std::string> cmds{};

		for (auto c : std::views::split(input, ' '))
		{
			cmds.push_back(std::string(std::string_view(c)));
		}

		if (cmds.empty())
			co_return errc::invalid_cmd;

		auto& cmd_str = *cmds.begin();

		//auto cmd_ptr = CMD.find(cmd_str);

		//if (!cmd_ptr)
		//	co_return errc::invalid_cmd;

		//if (!cmd_ptr->cmd_ptr)
		//	co_return errc::invalid_cmd;

		//cmd_ptr->cmd_ptr->load_options(cmds);

		aquarius::error_code ec;

		//try
		//{
		//	ec = co_await cmd_ptr->opt_func(response().body().output);
		//}
		//catch (std::exception& ec)
		//{
		//	XLOG_ERROR() << ec.what();
		//}

		response().result(ec.value());

		co_return errc::success;
	}
} // namespace serviced
