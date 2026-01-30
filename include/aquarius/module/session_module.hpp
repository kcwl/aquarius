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
		session_module(io_context& io, const std::string& name)
			: no_config_module<session_module<Session>>(io, name)
		{}

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

	template <typename Session>
	inline auto mpc_insert_session(std::shared_ptr<Session> session_ptr) -> awaitable<bool>
	{
		co_return co_await mpc::call<bool, session_module<Session>>([&](session_module<Session>* ptr) -> awaitable<bool>
																	{ co_return ptr->insert(session_ptr); });
	}

	template <typename Session>
	inline auto mpc_erase_session(std::size_t id) -> awaitable<void>
	{
		co_return co_await mpc::call<void, session_module<Session>>([&](session_module<Session>* ptr) -> awaitable<void>
																	{ co_return ptr->erase(id); });
	}

	template <typename Session>
	inline auto mpc_find_session(std::size_t id) -> awaitable<std::shared_ptr<Session>>
	{
		co_return co_await mpc::call<std::shared_ptr<Session>, session_module<Session>>(
			[&](session_module<Session>* ptr) -> awaitable<std::shared_ptr<Session>> { co_return ptr->find(id); });
	}
} // namespace aquarius