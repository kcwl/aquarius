#pragma once
#include <map>
#include <memory>
#include <mutex>

namespace aquarius
{
	template <typename Session, typename Area>
	class basic_cross_list
	{
	public:
		template <typename... Args>
		void append(std::shared_ptr<Session> session, Args&&... args)
		{
			std::lock_guard lk(mutex_);

			auto iter = sections_.find(session->id);
			if (iter != sections_.end())
				return;

			sections_[session->id] = session;

			area_.append(session->id, std::forward<Args>(args)...);
		}

		void remove(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			auto iter = sections_.find(id);

			if (iter == sections_.end())
				return;

			sections_.erase(iter);

			area_.remove(id);
		}

		template <typename... Args>
		void update(std::size_t id, Args&&... args)
		{
			std::lock_guard lk(mutex_);

			auto iter = sections_.find(id);

			if (iter == sections_.end())
				return;

			area_.update(id, std::forward<Args>(args)...);
		}

		template <typename BroadMsg>
		void broad(std::size_t id, BroadMsg msg)
		{
			std::lock_guard lk(mutex_);

			auto sessions = area_.find_area(id);

			for (auto& s : sessions)
			{
				if (!s)
					continue;

				s->write(msg);
			}
		}

	private:
		std::mutex mutex_;

		std::map<std::size_t, std::shared_ptr<Session>> sections_;

		Area area_;
	};
} // namespace aquarius