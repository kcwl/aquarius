#pragma once
#include <mutex>
#include <unordered_map>

namespace aquarius
{
	namespace impl
	{
		template <typename _Key, typename _Value>
		class router
		{
		public:
			router() = default;

			virtual ~router() = default;

		public:
			void regist(_Key key, _Value value)
			{
				std::lock_guard lk(mutex_);

				map_invokes_.insert({ key, value });
			}

		private:
			router(const router&) = delete;

			router(router&&) = delete;

		protected:
			std::mutex mutex_;

			std::unordered_map<_Key, _Value> map_invokes_;
		};
	} // namespace impl
} // namespace aquarius
