#pragma once
#include <aquarius/singleton.hpp>
#include <map>
#include <memory>
#include <mutex>

namespace aquarius
{
	namespace session
	{
		namespace detail
		{
			template <typename Session>
			class basic_store : public singleton<basic_store<Session>>
			{
			public:
				using value_type = Session;

			public:
				void insert(std::shared_ptr<Session> session)
				{
					std::lock_guard lk(mutex_);

					auto iter = sessions_.find(session->id());

					if (iter != sessions_.end())
						return;

					sessions_.emplace(session->id(), session);
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

			private:
				std::mutex mutex_;

				std::map<std::size_t, std::shared_ptr<Session>> sessions_;
			};

		} // namespace detail
	} // namespace session
} // namespace aquarius