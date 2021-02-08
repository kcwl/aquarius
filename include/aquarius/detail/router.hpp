#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "streambuf.hpp"


namespace aquarius
{
	namespace detail
	{
		template<class Func>
		struct invoker
		{
			static inline void apply(const Func& func,const detail::streambuf& buf,detail::streambuf& os_buf)
			{
				auto ctx = func();

				ctx->attach(buf,os_buf);
			}
		};

		class router
		{
		public:
			static router& get()
			{
				static router router_;
				return router_;
			}

			virtual ~router()
			{
			}

			template<class Func>
			void regist(const std::string& key, Func&& func)
			{
				map_invokes_.insert({key,std::bind(&invoker<Func>::apply,std::forward<Func>(func))});
			}

			void route(const std::string& key, const streambuf& buf,streambuf& os_buf)
			{
				auto iter = map_invokes_.find(key);
				if (iter == map_invokes_.end())
					return;

				iter->second(buf,os_buf);
			}

		private:
			router() = default;

			router(const router&) = delete;

			router(router&&) = delete;

			std::unordered_map<std::string, std::function<void(const streambuf& buf,streambuf& os_buf)>> map_invokes_;
		};
	}
}

