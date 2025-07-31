#pragma once
#include <aquarius/singleton.hpp>
#include <string>
#include <unordered_map>
#include <mutex>
#include <string>
#include <aquarius/cmd_options.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/awaitable.hpp>

namespace serviced
{
	struct cmd_info
	{
		cmd_info(const std::string& desc)
			: opt_func()
			, cmd_ptr(new aquarius::cmd_options(desc))
		{

		}

		std::function<aquarius::awaitable<aquarius::error_code>(std::string&)> opt_func;

		std::shared_ptr<aquarius::cmd_options> cmd_ptr;
	};

	class cmd_store : public aquarius::singleton<cmd_store>
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
}

#define CMD serviced::cmd_store::get_mutable_instance()