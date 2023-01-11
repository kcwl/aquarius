#pragma once
#include <aquarius/detail/singleton.hpp>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

namespace aquarius
{
	namespace detail
	{
		template <typename _R, typename... _Args>
		class router
		{
		public:
			router() = default;

		public:
			void regist(const std::string& key, std::function<_R(_Args...)>&& func)
			{
				std::lock_guard lk(mutex_);

				map_invokes_.insert({ key, std::forward<std::function<_R(_Args...)>>(func) });
			}

			_R invoke(const std::string& key, _Args&... args)
			{
				std::lock_guard lk(mutex_);

				auto iter = map_invokes_.find(key);

				if (iter == map_invokes_.end())
					return _R();

				return iter->second(args...);
			}

		private:
			router(const router&) = delete;

			router(router&&) = delete;

			std::unordered_map<std::string, std::function<_R(_Args...)>> map_invokes_;

			std::mutex mutex_;
		};

		template <typename _R, typename... _Args>
		class single_router : public router<_R, _Args...>, public detail::singleton<router<_R, _Args...>>
		{};
	} // namespace detail
} // namespace aquarius
