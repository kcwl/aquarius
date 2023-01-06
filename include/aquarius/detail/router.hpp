#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "singleton.hpp"

namespace aquarius
{
	namespace core
	{
		template<typename _Return, typename... _Args>
		class router
		{
		public:
			router() = default;

		public:
			template<typename _Func>
			void regist(const std::string& key, _Func&& func)
			{
				std::lock_guard lk(mutex_);

				map_invokes_.insert({ key, std::forward<_Func>(func) });
			}

			_Return invoke(const std::string& key, _Args&&... args)
			{
				std::lock_guard lk(mutex_);

				auto iter = map_invokes_.find(key);

				if (iter == map_invokes_.end())
					return _Return{};

				return iter->second(std::forward<_Args>(args)...);
			}

		private:
			router(const router&) = delete;

			router(router&&) = delete;

			std::unordered_map<std::string, std::function<_Return(_Args...)>> map_invokes_;

			std::mutex mutex_;
		};

		template<typename _Return, typename... _Args>
		class single_router
			: public router<_Return, _Args...>
			, public core::singleton<router<_Return, _Args...>>
		{

		};
	}
}

