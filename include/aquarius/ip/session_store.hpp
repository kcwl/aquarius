#pragma once
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/singleton.hpp>
#include <shared_mutex>

namespace aquarius
{
	class session_store : public singleton<session_store>
	{
	public:
		using func_type = std::function<asio::awaitable<void>(flex_buffer&)>;

	public:
		auto invoke(std::size_t session_id, flex_buffer& buffer) ->asio::awaitable<void>
		{
			std::shared_lock lk(mutex_);

			auto iter = sessions_.find(session_id);
			if (iter == sessions_.end())
			{
				co_return;
			}

			co_await iter->second(buffer);
		}

		bool put(std::size_t session_id, const func_type& f)
		{
			std::unique_lock lk(mutex_);

			auto [_, result] = sessions_.insert({ session_id, f });

			return result;
		}

	private:
		std::shared_mutex mutex_;

		std::unordered_map<std::size_t, func_type> sessions_;
	};

	inline auto mpc_invoke_session(std::size_t session_id, flex_buffer& buffer)->asio::awaitable<void>
	{
		co_return co_await session_store::get_mutable_instance().invoke(session_id, buffer);
	}

	inline bool mpc_put_session(std::size_t session_id, const session_store::func_type& f)
	{
		return session_store::get_mutable_instance().put(session_id, f);
	}
} // namespace aquarius