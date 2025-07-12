#pragma once
#include <string_view>
#include "sdtree.h"
#include <aquarius/singleton.hpp>
#include <aquarius/session_store.hpp>

using namespace std::string_view_literals;

namespace serviced
{
	constexpr std::string_view root = "/root"sv;
	constexpr std::string_view channel = "/root/channel"sv;

	template <typename Protocol>
	class transfer_manager : public aquarius::singleton<transfer_manager<Protocol>>
	{
	public:
		transfer_manager() = default;

	public:
		bool channel_subscribe(const std::string& tpc, std::shared_ptr<sdnode> node_ptr)
		{
			if (!tree_.create_leaf(std::string(channel), tpc))
				return false;

			auto node = tree_.find_leaf(std::string(channel) + tpc);

			if (!node)
				return false;

			node->add_child(node_ptr);

			return true;
		}

		void channel_unsubscribe(const std::string& tpc, std::size_t id)
		{
			tree_.remove_leaf(std::string(channel.data()) + "/" + tpc + "/" + std::to_string(id));
		}

		template <typename... Args>
		void publish(const std::string& tpc, Args&&... args)
		{
			auto leaf = tree_.find_leaf(std::string(channel.data()) + "/" + tpc);

			if (!leaf)
				return;

			auto childs = leaf->get_childs();

			for (auto& child : childs)
			{
				auto id = child->id();

				auto session = aquarius::session::defer<Protocol>(id);

				if (!session)
					continue;

				session->async_send(std::forward<Args>(args)...);
			}
		}

	private:
		sdtree tree_;
	};
} // namespace serviced