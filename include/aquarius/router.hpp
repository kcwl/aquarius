#pragma once
#include <aquarius/context.hpp>
#include <aquarius/detail/router.hpp>
#include <aquarius/impl/io.hpp>

namespace aquarius
{
	namespace impl
	{
		class session;
		
		template <typename _Streambuf>
		class basic_message;

		using xmessage = basic_message<flex_buffer_t>;
	}
}


namespace aquarius
{
	using ctx_router = core::single_router<int, std::shared_ptr<impl::session>, std::shared_ptr<impl::xmessage>>;

	template <typename _Context>
	struct ctx_regist
	{
		ctx_regist(const std::size_t& key)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			ctx_router::instance().regist(
				_key, []<typename _Session, typename _Message>(std::shared_ptr<_Session> session_ptr,
															   std::shared_ptr<_Message> req_ptr)
				{ std::make_shared<_Context>(session_ptr)->visit(req_ptr); });
		}
	};

	struct invoke_helper
	{
		template <typename... _Args>
		static int invoke(uint32_t key, _Args&&... args)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			return aquarius::ctx_router::instance().invoke(_key, std::forward<_Args>(args)...);
		}
	};

	template <typename _Message>
	using msg_router = core::single_router<_Message>;

	template <typename _BaseMessage, typename _Message>
	struct msg_regist
	{
		msg_regist(const std::size_t& key)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			msg_router<_BaseMessage>::instance().regist(_key, [] { return std::make_shared<_Message>(); });
		}
	};
	template <typename _Message, typename... _Args>
	struct invoke_msg_helper
	{
		static _Message invoke(uint32_t key, _Args&&... args)
		{
			std::string _key = "aquarius_" + std::to_string(key);

			return aquarius::msg_router<_Message>::instance().invoke(_key, std::forward<_Args>(args)...);
		}
	};

#define MESSAGE_DEFINE(base_type, type) static msg_regist<base_type, type> req##type(type::Number);
#define CONTEXT_DEFINE(base_type, type) static ctx_regist<base_type, type> ctx##type(type::Number);
} // namespace aquarius