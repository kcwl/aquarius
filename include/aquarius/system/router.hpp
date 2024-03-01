#pragma once
#include <aquarius/system/singleton.hpp>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

namespace aquarius
{
	namespace system
	{
		template <typename _R, typename... _Args>
		class router
		{
		public:
			router() = default;

		public:
			void regist(std::size_t key, std::function<_R(_Args...)>&& func)
			{
				std::lock_guard lk(mutex_);

				map_invokes_.insert({ key, std::forward<std::function<_R(_Args...)>>(func) });
			}

			_R invoke(std::size_t key, _Args&... args)
			{
				std::lock_guard lk(mutex_);

				auto iter = map_invokes_.find(key);

				if (iter == map_invokes_.end())
					return _R();

				return iter->second(args...);
			}

		protected:
			std::unordered_map<std::size_t, std::function<_R(_Args...)>> map_invokes_;

			std::mutex mutex_;

		private:
			router(const router&) = delete;

			router(router&&) = delete;
		};

		template <typename _Single, typename _R, typename... _Args>
		class single_router : public router<_R, _Args...>, public singleton<_Single>
		{};
	} // namespace system
} // namespace aquarius
