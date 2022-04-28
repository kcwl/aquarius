#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "singleton.hpp"

namespace aquarius
{
	namespace detail
	{
		template<typename Func, typename... Args>
		struct invoker;

		template<typename R, typename... Args>
		class router
		{
		public:
			router() = default;

		public:
			void regist(const std::string& key, std::function<R(Args...)>&& func)
			{
				std::lock_guard lk(mutex_);

				map_invokes_.insert({ key,std::forward<std::function<R(Args...)>>(func) });
			}

			R invoke(const std::string& key, Args&&... args)
			{
				std::lock_guard lk(mutex_);

				auto iter = map_invokes_.find(key);

				if (iter == map_invokes_.end())
					return R();

				return iter->second(std::forward<Args>(args)...);
			}

		private:
			router(const router&) = delete;

			router(router&&) = delete;

			std::unordered_map<std::string, std::function<R(Args...)>> map_invokes_;

			std::mutex mutex_;
		};

		template<typename R, typename... Args>
		class single_router
			: public router<R, Args...>
			, public detail::singleton<router<R, Args...>>
		{

		};
	}
}

