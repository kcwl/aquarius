#pragma once
#include <aquarius/module/module.hpp>
#include <map>
#include <mutex>

namespace aquarius
{
	template <typename Session>
	class session_module : public no_config_module<session_module<Session>>
	{
	public:
		session_module(const std::string& name)
			: no_config_module<session_module<Session>>(name)
		{

		}

	public:
		bool insert(std::shared_ptr<Session> session)
		{
			std::lock_guard lk(mutex_);
			auto iter = sessions_.find(session->uuid());
			if (iter != sessions_.end())
				return false;
			sessions_.emplace(session->uuid(), session);

			return true;
		}
		void erase(std::size_t id)
		{
			std::lock_guard lk(mutex_);
			sessions_.erase(id);
		}
		std::shared_ptr<Session> find(std::size_t id)
		{
			std::lock_guard lk(mutex_);
			auto iter = sessions_.find(id);
			if (iter == sessions_.end())
				return nullptr;

			return iter->second;
		}

		std::size_t size() const
		{
			return sessions_.size();
		}

	private:
		std::mutex mutex_;

		std::map<std::size_t, std::shared_ptr<Session>> sessions_;
	};
} // namespace aquarius