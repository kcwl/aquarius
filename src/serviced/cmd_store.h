#pragma once
#include <aquarius/singleton.hpp>
#include <string>
#include <map>
#include <mutex>
#include <aquarius/cmd_options.hpp>

namespace serviced
{
	class cmd_store : public aquarius::singleton<cmd_store>
	{
	public:
		cmd_store() = default;

	public:
		void insert(std::shared_ptr<aquarius::cmd_options> cmd);

		std::map<std::string, std::shared_ptr<aquarius::cmd_options>> cmds();

		std::shared_ptr<aquarius::cmd_options> find(const std::string& cmd);

	private:
		std::mutex mutex_;

		std::map<std::string, std::shared_ptr<aquarius::cmd_options>> cmds_;
	};
}

#define CMD serviced::cmd_store::get_mutable_instance()