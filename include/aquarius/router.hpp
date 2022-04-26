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

			return ctx_ptr;
		}
	};

	template<typename Request, typename Context>
	void dispatch(std::shared_ptr<Request> msg_ptr, ftstream& buf)
	{
		try
		{
			buf >> *msg_ptr;
		}
		catch (...)
		{
			return;
		}

		msg_ptr->accept(ctx_ptr);
	}

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

	using ctx_router = detail::router<std::shared_ptr<context>>;

	using msg_router = detail::router<void, ftstream&>;

	template<typename Basic, typename Context>
	struct ctx_regist
	{
		ctx_regist(const std::size_t& key)
		{
			std::string _key = "ctx_" + std::to_string(key);

			auto f = [] {return std::dynamic_pointer_cast<Basic>(std::make_shared<Context>()); };

			ctx_router::instance().regist(_key, std::bind(&invoker<true, std::shared_ptr<Basic>, decltype(f)>::apply, f));
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
				std::bind(&invoker<false,void, decltype(f), ftstream&>::apply, f, std::placeholders::_1));
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
			return ctx_router::instance().invoke(key);
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