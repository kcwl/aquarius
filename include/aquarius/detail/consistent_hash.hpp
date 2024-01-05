#pragma once
#include <format>
#include <set>
#include <string>
#include <map>

namespace aquarius
{
	class consistent_hash
	{
		constexpr static std::size_t virtual_number = 100;

	public:
		consistent_hash() = default;
		~consistent_hash() = default;

	public:
		void add(const std::string& ip,int32_t port)
		{
			auto addr = std::format("{}:{}", ip, port);

			physic_nodes_.insert(addr);

			virtual_impl(addr);
		}

		void erase(const std::string& ip)
		{
			physic_nodes_.erase(ip);

			for (std::size_t i = 0; i < virtual_number; ++i)
			{
				auto hash_key = std::hash<std::string>()(std::format("{}@{}", ip, i));

				auto iter = server_nodes_.find(hash_key);
				if (iter == server_nodes_.end())
					continue;

				server_nodes_.erase(iter);
			}
		}

		void virtual_node()
		{
			for (const auto& ip : physic_nodes_)
			{
				virtual_impl(ip);
			}
		}

		std::string get(const std::string& ip)
		{
			if (server_nodes_.empty())
				return {};

			auto hash_key = std::hash<std::string>()(ip);

			auto iter = server_nodes_.lower_bound(hash_key);

			if (iter == server_nodes_.end())
				return server_nodes_.begin()->second;

			return iter->second;
		}

	private:
		void virtual_impl(const std::string& ip)
		{
			for (std::size_t i = 0; i < virtual_number; ++i)
			{
				auto hash_key = std::hash<std::string>()(std::format("{}@{}", ip, i));

				server_nodes_.emplace(hash_key, ip);
			}
		}

	private:
		std::set<std::string> physic_nodes_;

		std::map<std::size_t, std::string> server_nodes_;
	};
} // namespace aquarius