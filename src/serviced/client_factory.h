#pragma once
#include <aquarius/executor/tcp_client.hpp>
#include <aquarius/singleton.hpp>
#include <mutex>

namespace serviced
{
	class client_factory : public aquarius::singleton<client_factory>
	{
	public:
		using client = aquarius::tcp_client;

	public:
		client_factory() = default;

	public:
		void insert(const std::string& ip_addr, std::shared_ptr<client> client_ptr);

		std::shared_ptr<client> find(const std::string& ip_addr);

		std::vector<std::shared_ptr<client>> get_client();

		void remove(const std::string& ip_addr);

	private:
		std::mutex mutex_;

		std::map<std::string, std::shared_ptr<client>> clients_;
	};
} // namespace serviced

#define CLIENT serviced::client_factory::get_mutable_instance()