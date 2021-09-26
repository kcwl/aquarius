#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "detail/singleton.hpp"
#include "common.hpp"

namespace aquarius
{
	class context;
	class connect;
	class schedule;

	class router : public detail::singleton<router>
	{
	public:
		router() {}

	public:
		template<class Func>
		void regist_invoke(const std::string& key, Func&& func)
		{
			map_invokes_.insert({key,std::bind(&invoker<Func>::apply,std::forward<Func>(func),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
		}

		template<typename Func>
		void regist_func(const std::string& key, Func&& f)
		{
			map_funcs_.insert({key, std::forward<Func>(f)});
		}

		void route_invoke(const std::string& key, streambuf& buf, std::shared_ptr<schedule> schedule_ptr, std::shared_ptr<context> ctx_ptr)
		{
			auto iter = map_invokes_.find(key);
			if(iter == map_invokes_.end())
				return;

			iter->second(buf, schedule_ptr, ctx_ptr);
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

		std::unordered_map<std::string, std::function<void(streambuf&, std::shared_ptr<schedule>, std::shared_ptr<context> ctx_ptr)>> map_invokes_;

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

