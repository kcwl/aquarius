#pragma once
#include <aquarius/session/session.hpp>
#include <aquarius/detail/singleton.hpp>
#include <mutex>


namespace aquarius
{
	namespace detail
	{
		class session_manager : public singleton<session_manager>
		{
		public:
			bool push(std::shared_ptr<basic_session> session_ptr)
			{
				std::lock_guard lk(mutex_);

				sessions_.emplace(session_ptr->uid(), session_ptr);

				return true;
			}

			bool erase(std::size_t id)
			{
				std::lock_guard lk(mutex_);

				return sessions_.erase(id) != 0;
			}

			std::shared_ptr<basic_session> find(std::size_t id)
			{
				std::lock_guard lk(mutex_);

				auto iter = sessions_.find(id);

				if (iter == sessions_.end())
					return nullptr;
				
				return iter->second;
			}

			std::size_t count()
			{
				return sessions_.size();
			}

			void broadcast(flex_buffer_t&& buffer)
			{
				std::lock_guard lk(mutex_);

				for (auto& session : sessions_)
				{
					if (!session.second)
						continue;

					session.second->async_write(std::move(buffer));
				}
			}

			template<typename _Func>
			void broadcast(flex_buffer_t&& buffer, _Func&& f)
			{
				std::lock_guard lk(mutex_);

				for (auto& session : sessions_)
				{
					if (!session.second)
						continue;

					if (!std::forward<_Func>(f)(session.second))
						continue;

					session.second->async_write(std::forward<flex_buffer_t>(buffer));
				}
			}

			void send_someone(std::size_t uid, flex_buffer_t&& buffer)
			{
				std::lock_guard lk(mutex_);

				auto iter = sessions_.find(uid);

				if (iter == sessions_.end())
					return;

				if (!iter->second)
					return;

				iter->second->async_write(std::move(buffer));
			}

		private:
			std::unordered_map<std::size_t, std::shared_ptr<basic_session>> sessions_;

			std::mutex mutex_;
		};
	}
}