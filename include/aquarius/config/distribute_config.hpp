#pragma once
#include <aquarius/detail/singleton.hpp>
#include <boost/json.hpp>
#include <filesystem>
#include <fstream>

namespace aquarius
{
	struct ip_addr
	{
		std::string ip_addr;
		int32_t port;
	};

	class distribute_config : public aquarius::detail::singleton<distribute_config>
	{
	public:
		distribute_config()
			: server_port_()
			, pool_size_(0)
			, master_()
			, router_()
			, balance_()
			, master_addr_()
			, routers_()
			, file_name_()
		{}

		~distribute_config() = default;

	public:
		bool load(const std::string& file_name)
		{
			boost::json::stream_parser parse{};

			std::fstream f(file_name, std::ios::in);
			if (!f.is_open())
				return false;

			file_name_ = file_name;

			std::size_t size = static_cast<std::size_t>(std::filesystem::file_size(file_name_));

			std::string lines{};
			lines.resize(size);

			f.read(lines.data(), size);

			if (parse.write(lines.data(), size) != size)
				return false;

			auto json_value = parse.release();

			auto object = json_value.if_object();

			if (!object)
				return false;

			auto& setting = object->at("setting");

			server_port_ = static_cast<int>(setting.as_object().at("port").as_int64());
			slave_port_ = static_cast<int>(setting.as_object().at("slave_port").as_int64());
			pool_size_ = static_cast<int>(setting.as_object().at("threads").as_int64());
			master_ = setting.as_object().at("master").as_bool();
			router_ = setting.as_object().at("router").as_bool();

			auto& master = object->at("master");
			balance_ = master.as_object().at("balance").as_bool();

			auto& slave = object->at("slave");
			master_addr_.ip_addr = slave.at("master").as_object().at("ip").as_string();
			master_addr_.port = static_cast<int32_t>(slave.at("master").as_object().at("port").as_int64());

			auto& router = object->at("router");

			for (auto& addr : router.as_array())
			{
				routers_.push_back({});
				auto& back = routers_.back();

				back.ip_addr = addr.as_object().at("ip").as_string();
				back.port = static_cast<int32_t>(addr.as_object().at("port").as_int64());
			}

			return true;
		}

	public:
		int server_port_;

		int slave_port_;

		int pool_size_;

		bool master_;

		bool router_;

		bool balance_;

		ip_addr master_addr_;

		std::vector<ip_addr> routers_;

	private:
		std::string file_name_;
	};
} // namespace aquarius

#define APP_CONFIG aquarius::distribute_config::instance()