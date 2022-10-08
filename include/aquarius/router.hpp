#pragma once
#include "context.hpp"
#include "session.hpp"
#include "core/router.hpp"
#include "stream/streambuf.hpp"


namespace aquarius
{
	template<typename _Session>
	using ctx_router = core::single_router<void, std::shared_ptr<_Session>, ftstream&>;

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
}