#pragma once
#include <aquarius/executor/tcp_client.hpp>
#include <aquarius/singleton.hpp>
#include <mutex>
#include <list>

namespace serviced
{
	class client_factory : public aquarius::singleton<client_factory>
	{
	public:
		using client = aquarius::tcp_client;

	public:
		client_factory() = default;

	public:
		void insert(std::size_t topic, const std::string& host, const std::string& port, std::shared_ptr<client> client_ptr);

		std::shared_ptr<client> find(std::size_t topic, const std::string& host, const std::string& port);

		std::vector<std::shared_ptr<client>> get_client();

		void remove(std::size_t topic, const std::string& host, const std::string& port);

	private:
		std::mutex mutex_;

		std::map<std::size_t, std::list<std::shared_ptr<client>>> clients_;
	};
} // namespace serviced

#define CLIENT serviced::client_factory::get_mutable_instance()