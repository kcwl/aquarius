#pragma once
#include <aquarius.hpp>
#include <mutex>
#include <string>
#include <unordered_map>

namespace serviced
{
	struct cmd_info
	{
		cmd_info(const std::string& desc)
			: opt_func()
			, cmd_ptr(new aquarius::cmd_options(desc))
		{}

		std::function<aquarius::awaitable<aquarius::error_code>(std::string&)> opt_func;

		std::shared_ptr<aquarius::cmd_options> cmd_ptr;
	};

	class cmd_store
	{
	public:
		cmd_store() = default;

	public:
		void insert(std::shared_ptr<cmd_info> cmd);

		std::unordered_map<std::string, std::shared_ptr<cmd_info>> cmds();

		std::shared_ptr<cmd_info> find(const std::string& cmd);

	private:
		std::mutex mutex_;

		std::unordered_map<std::string, std::shared_ptr<cmd_info>> cmds_;
	};
} // namespace serviced