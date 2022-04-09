#pragma once
#include "detail/router.hpp"
#include "type_traits.hpp"
#include "dispatch.hpp"
#include "message/message.hpp"

namespace aquarius
{
	class context;

	class session;

	template<bool Token, typename R, typename Func, typename... Args>
	struct invoker;

	template<typename R, typename Func, typename... Args>
	struct invoker<true, R, Func, Args...>
	{
		static inline R apply(const Func& func, Args&&... args)
		{
			auto ctx_ptr = func();

			if (ctx_ptr == nullptr)
				return nullptr;

			ctx_ptr->attach_session(std::forward<Args>(args)...);

			return ctx_ptr;
		}
	};

	template<typename R, typename Func, typename... Args>
	struct invoker<false, R, Func, Args...>
	{
		static inline R apply(const Func& func, Args&&... args)
		{
			auto msg_ptr = func();

			if (msg_ptr == nullptr)
				return;

			return dispatch(msg_ptr, std::forward<Args>(args)...);
		}
	};

	using ctx_router = detail::router<std::shared_ptr<context>,std::shared_ptr<session>>;

	using msg_router = detail::router<void, std::shared_ptr<context>, msg::header_value, ftstream&>;

	template<typename Basic, typename Context>
	struct ctx_regist
	{
		ctx_regist(const std::size_t& key)
		{
			std::string _key = "ctx_" + std::to_string(key);

			auto f = [] {return std::dynamic_pointer_cast<Basic>(std::make_shared<Context>()); };

			ctx_router::instance().regist(_key, std::bind(&invoker<true, std::shared_ptr<Basic>, decltype(f), std::shared_ptr<session>>::apply, f, std::placeholders::_1));
		}
	};

	template<typename Message>
	struct msg_regist
	{
		msg_regist()
		{
			std::string _key = "msg_" + std::to_string(Message::NUMBER);

			auto f = [] {return std::make_shared<Message>(); };

			msg_router::instance().regist(_key, 
				std::bind(&invoker<false,void, decltype(f), std::shared_ptr<context>, msg::header_value, ftstream&>::apply, f, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		}
	};

	template<bool Token, typename R, typename... Args>
	struct invoke_helper
	{
		static inline R invoke(const std::string& key, Args&&... args) {}
	};

	template<typename R, typename... Args>
	struct invoke_helper<true, R, Args...>
	{
		static inline R invoke(const std::string& key, Args&&... args)
		{
			return ctx_router::instance().invoke(key, std::forward<Args>(args)...);
		}
	};

	template<typename R, typename... Args>
	struct invoke_helper<false, R, Args...>
	{
		static inline R invoke(const std::string& key, Args&&... args)
		{
			return msg_router::instance().invoke(key, std::forward<Args>(args)...);
		}
	};
}