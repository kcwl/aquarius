#pragma once
#include <aquarius/channel/channel.hpp>
#include <unordered_map>

namespace aquarius
{
	class group
	{
	public:
		group() = default;
		~group() = default;

	public:
		bool subscribe(const std::string& topic, std::shared_ptr<xsession> session_ptr)
		{
			std::lock_guard lk(mutex_);

			std::shared_ptr<channel> channel_ptr;

			auto iter = channels_.find(topic);

			if (iter == channels_.end())
			{
				channels_.emplace(topic, channel_ptr);

				channel_ptr = make_channel(topic);
			}
			else
			{
				channel_ptr = iter->second;
			}

			channel_ptr->subscribes_.push_back(session_ptr);
		}

		void publish(const std::string& topic, std::any&& anything)
		{
			std::lock_guard lk(mutex_);

			auto iter = channels_.find(topic);
			
			if (iter == channels_.end())
				return;

			if (!iter->second)
				return;

			iter->second->anythings_ = std::move(anything);
		}

	private:
		auto make_channel(const std::string& topic) -> std::shared_ptr<channel>
		{
			return std::make_shared<channel>(topic);
		}

		bool exist(const std::string& topic)
		{
			auto iter = channels_.find(topic);

			if (iter == channels_.end())
				return false;

			return true;
		}

	public:
		std::mutex mutex_;

		std::unordered_map<std::string, std::shared_ptr<channel>> channels_;
	};
} // namespace aquarius