#include "cmd_store.h"

namespace serviced
{
	void cmd_store::insert(std::shared_ptr<cmd_info> cmd)
	{
		std::lock_guard lk(mutex_);

		cmds_[cmd->cmd_ptr->desc()] = cmd;
	}

	std::map<std::string, std::shared_ptr<cmd_info>> cmd_store::cmds()
	{
		std::lock_guard lk(mutex_);

		return cmds_;
	}

	std::shared_ptr<cmd_info> cmd_store::find(const std::string& cmd)
	{
		std::lock_guard lk(mutex_);

		auto iter = cmds_.find(cmd);

		if (iter == cmds_.end())
			return nullptr;

		return iter->second;
	}
}