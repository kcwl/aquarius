#pragma once
#include "detail/router.hpp"
#include "type_traits.hpp"
#include "stream.hpp"
#include "message/null_message.hpp"


namespace aquarius
{
	template<bool Token, typename Func, typename... Args>
	struct invoker;

	template<typename Func, typename... Args>
	struct invoker<true, Func, Args...>
	{
		static inline decltype(auto) apply(const Func& func, Args&&... args)
		{
			auto ctx_ptr = func();

			return ctx_ptr;
		}
	};

	template<typename Request>
	auto dispatch(std::shared_ptr<Request> msg_ptr, aquarius::ftstream& buf)
	{
		if(!msg_ptr->parse_bytes(buf))
			msg_ptr = nullptr;

		return msg_ptr;
	}

	template<typename Func, typename... Args>
	struct invoker<false, Func, Args...>
	{
		static inline decltype(auto) apply(const Func& func, Args&&... args)
		{
			auto msg_ptr = func();

			return dispatch(msg_ptr, std::forward<Args>(args)...);
		}
	};

	using ctx_router = detail::single_router<std::shared_ptr<context>>;

	using msg_router = detail::single_router<std::shared_ptr<null_message>, aquarius::ftstream&>;

	template<typename Context>
	struct ctx_regist
	{
		ctx_regist(const std::size_t& key)
		{
			std::string _key = "ctx_" + std::to_string(key);

			auto f = [] {return std::dynamic_pointer_cast<context>(std::make_shared<Context>()); };

			ctx_router::instance().regist(_key, std::bind(&invoker<true, decltype(f)>::apply, f));
		}
	};

	template<typename Message>
	struct msg_regist
	{
		msg_regist()
		{
			std::string _key = "msg_" + std::to_string(Message::NUMBER);

			auto f = [] {return std::dynamic_pointer_cast<null_message>(std::make_shared<Message>()); };

			msg_router::instance().regist(_key, 
				std::bind(&invoker<false, decltype(f), ftstream&>::apply, f, std::placeholders::_1));
		}
	};

	template<bool Token>
	struct invoke_helper
	{
		template<typename... Args>
		static inline decltype(auto) invoke(const std::string& key, Args&&... args) 
		{
			return std::string{};
		}
	};

	template<>
	struct invoke_helper<true>
	{
		template<typename... Args>
		static inline decltype(auto) invoke(const std::string& key, Args&&... args)
		{
			return ctx_router::instance().invoke(key, std::forward<Args>(args)...);
		}
	};

	template<>
	struct invoke_helper<false>
	{
		template<typename... Args>
		static inline decltype(auto) invoke(const std::string& key, Args&&... args)
		{
			return msg_router::instance().invoke(key, std::forward<Args>(args)...);
		}
	};
}