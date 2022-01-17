#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "singleton.hpp"
#include "easybuffers/include/easybuffers.hpp"

namespace aquarius
{
	class context;

	namespace detail
	{
		using streambuf = easybuffers::ebstream<>;

		template<class Func>
		struct invoker
		{
			static inline void apply(const Func& func, streambuf& buf)
			{
				auto msg_ptr = func();

				if(msg_ptr == nullptr)
					return;

				msg_ptr->parse_bytes(buf);
			}
		};

		class router : public singleton<router>
		{
		public:
			router() {}

		public:
			template<class Func>
			void regist_invoke(const std::string& key, Func&& func)
			{
				map_invokes_.insert({key,std::bind(&invoker<Func>::apply,std::forward<Func>(func),std::placeholders::_1)});
			}

			template<typename Func>
			void regist_func(const std::string& key, Func&& f)
			{
				map_funcs_.insert({key, std::forward<Func>(f)});
			}

			void route_invoke(const std::string& key, streambuf& buf)
			{
				auto iter = map_invokes_.find(key);
				if (iter == map_invokes_.end())
					return;

				iter->second(buf);
			}

			std::shared_ptr<context> route_func(const std::string& key)
			{
				auto iter = map_funcs_.find(key);
				if(iter == map_funcs_.end())
					return nullptr;

				return iter->second();
			}

		private:
			router(const router&) = delete;

			router(router&&) = delete;

			std::unordered_map<std::string, std::function<void(streambuf&)>> map_invokes_;

			std::unordered_map<std::string, std::function<std::shared_ptr<context>()>> map_funcs_;
		};

		template<typename Request>
		struct msg_regist
		{
			msg_regist(const std::string& key)
			{
				std::string _key = "msg_" + key;
				router::instance().regist_invoke(_key, []()
												 {
													  return std::make_shared<Request>();
												 });
			}
		};

		template<typename Context>
		struct ctx_regist
		{
			ctx_regist(const std::string& key)
			{
				std::string _key = "ctx_" + key;
				router::instance().regist_func(_key, []()
											   {
												   return std::make_shared<Context>();
											   });
			}
		};
	}
}

