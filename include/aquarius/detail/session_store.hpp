#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/singleton.hpp>
#include <mutex>
#include <map>

namespace aquarius
{
	namespace detail
	{
		template <typename Session>
		class session_store : public singleton<session_store<Session>>
		{
		public:
			using value_type = Session;

		public:
			void insert(std::shared_ptr<Session> session)
			{
				std::lock_guard lk(mutex_);
				auto iter = sessions_.find(session->uuid());
				if (iter != sessions_.end())
					return;
				sessions_.emplace(session->uuid(), session);
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
				return iter->second.lock();
			}

			std::size_t size() const
			{
				return sessions_.size();
			}

		private:
			std::mutex mutex_;

			std::map<std::size_t, std::weak_ptr<Session>> sessions_;
		};

		template <typename Session>
		auto attach_session(std::size_t id)
		{
			return session_store<Session>::get_mutable_instance().find(id);
		}

		template <typename Session>
		void remove_session(std::size_t id)
		{
			return session_store<Session>::get_mutable_instance().erase(id);
		}

		template <typename Session>
		void regist_session(std::shared_ptr<Session> session)
		{
			return session_store<Session>::get_mutable_instance().insert(session);
		}

		template<typename Session>
		std::size_t session_storage()
		{
			return session_store<Session>::get_mutable_instance().size();
		}

	} // namespace detail
} // namespace aquarius