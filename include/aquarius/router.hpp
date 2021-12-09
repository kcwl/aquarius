#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "context.hpp"
#include "schedule.hpp"
#include "detail/singleton.hpp"
#include "common.hpp"

namespace aquarius
{
	class connect;
	class schedule;

	template<typename _Ty>
	struct unpack_request
	{
		static inline std::shared_ptr<_Ty> pack(streambuf& buf)
		{
			auto request_ptr = std::make_shared<_Ty>();

			if (request_ptr == nullptr)
				return nullptr;

			request_ptr->parse_bytes(buf);

			return request_ptr;
		}
	};

	template<class Func>
	struct invoker
	{
		static inline void apply(const Func& func, streambuf& buf, send_response_t&& send_reponse)
		{
			auto ctx_ptr = func();

			if (ctx_ptr == nullptr)
				return;

			using Request = typename decltype(ctx_ptr)::element_type::request_t;

			auto msg_ptr = unpack_request<Request>::pack(buf);

			if (msg_ptr == nullptr)
				return;

			msg_ptr->accept(ctx_ptr, std::forward<send_response_t>(send_reponse));
		}
	};

	class router : public detail::singleton<router>
	{
	public:
		router() {}

	public:
		template<class Func>
		void regist_invoke(const std::string& key, Func&& func)
		{
			std::lock_guard lk(mutex_);

			map_invokes_.insert({ key,std::bind(&invoker<Func>::apply,std::forward<Func>(func),std::placeholders::_1,std::placeholders::_2) });
		}

		void route_invoke(const std::string& key, streambuf& buf, send_response_t&& f)
		{
			std::lock_guard lk(mutex_);

			auto iter = map_invokes_.find(key);

			if(iter == map_invokes_.end())
				return;

			return iter->second(buf, std::forward<send_response_t>(f));
		}

	private:
		router(const router&) = delete;

		router(router&&) = delete;

		std::unordered_map<std::string, std::function<void(streambuf&,  send_response_t)>> map_invokes_;

		std::mutex mutex_;
	};

	template<typename Context>
	struct ctx_regist
	{
		ctx_regist(const std::string& key)
		{
			std::string _key = "ctx_" + key;
			router::instance().regist_invoke(_key, []()
										   {
											   return std::make_shared<Context>();
										   });
		}
	};
}

