#pragma once
#include "core/router.hpp"
#include "stream.hpp"
#include "context.hpp"
#include "session.hpp"

namespace aquarius
{
	//template<bool Token, typename Func, typename... Args>
	//struct invoker;

	//template<typename Func, typename... Args>
	//struct invoker<true, Func, Args...>
	//{
	//	static inline decltype(auto) apply(const Func& func, Args&&... args)
	//	{
	//		auto ctx_ptr = func();

	//		return ctx_ptr;
	//	}
	//};

	//template<typename Request>
	//auto dispatch(std::shared_ptr<Request> msg_ptr, aquarius::ftstream& buf)
	//{
	//	if(!msg_ptr->parse_bytes(buf))
	//		msg_ptr = nullptr;

	//	return msg_ptr;
	//}

	//template<typename Func, typename... Args>
	//struct invoker<false, Func, Args...>
	//{
	//	static inline decltype(auto) apply(const Func& func, Args&&... args)
	//	{
	//		auto msg_ptr = func();

	//		return dispatch(msg_ptr, std::forward<Args>(args)...);
	//	}
	//};
	template<typename _Session>
	using ctx_router = core::single_router<void, std::shared_ptr<_Session>, ftstream&>;

	//using msg_router = detail::single_router<std::shared_ptr<null_message>, aquarius::ftstream&>;

	template<typename _Session, typename _Stream>
	struct invoke
	{
		template<typename _Service>
		static void dispatch(std::shared_ptr<_Session> session_ptr, _Stream& buffer)
		{
			_Service(session_ptr).accept(buffer);
		}
	};

	template<typename _Session, typename Context>
	struct ctx_regist
	{
		ctx_regist(const std::size_t& key)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			ctx_router<_Session>::instance().regist(_key, std::bind(&invoke<_Session, ftstream>::template dispatch<Context>, std::placeholders::_1, std::placeholders::_2));
		}
	};

	struct invoke_helper
	{
		template<typename _Session, typename... _Args>
		static void invoke(uint32_t key, std::shared_ptr<_Session>&& session_ptr, _Args&&... args)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			aquarius::ctx_router<_Session>::instance().invoke(_key, std::move(session_ptr), std::forward<_Args>(args)...);
		}
	};

	//template<typename Message>
	//struct msg_regist
	//{
	//	msg_regist()
	//	{
	//		std::string _key = "msg_" + std::to_string(Message::NUMBER);

	//		auto f = [] {return std::dynamic_pointer_cast<null_message>(std::make_shared<Message>()); };

	//		msg_router::instance().regist(_key, 
	//			std::bind(&invoker<false, decltype(f), ftstream&>::apply, f, std::placeholders::_1));
	//	}
	//};

	//template<bool Token>
	//struct invoke_helper
	//{
	//	template<typename... Args>
	//	static inline decltype(auto) invoke(const std::string& key, Args&&... args) 
	//	{
	//		return std::string{};
	//	}
	//};

	//template<>
	//struct invoke_helper<true>
	//{
	//	template<typename... Args>
	//	static inline decltype(auto) invoke(const std::string& key, Args&&... args)
	//	{
	//		return ctx_router::instance().invoke(key, std::forward<Args>(args)...);
	//	}
	//};

	//template<>
	//struct invoke_helper<false>
	//{
	//	template<typename... Args>
	//	static inline decltype(auto) invoke(const std::string& key, Args&&... args)
	//	{
	//		return msg_router::instance().invoke(key, std::forward<Args>(args)...);
	//	}
	//};
}