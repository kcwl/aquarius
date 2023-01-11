#pragma once
#include <aquarius/context.hpp>
#include <aquarius/detail/router.hpp>
#include <aquarius/impl/io.hpp>

namespace aquarius
{
	namespace impl
	{
		class session;
		class xmessage;
	} // namespace impl
} // namespace aquarius

namespace aquarius
{
	namespace impl
	{
		// using ctx_router = core::single_router<int, std::shared_ptr<impl::session>, flex_buffer_t&>;

		// template <typename _Context>
		// struct ctx_regist
		//{
		//	ctx_regist(const std::size_t& key)
		//	{
		//		std::string _key = "aquarius_" + std::to_string(key);

		//		ctx_router::instance().regist(
		//			_key,
		//			[&]<typename _Request>(std::shared_ptr<impl::session> session_ptr, _Request* req_ptr)
		//			{
		//				auto msg_ptr = invoke_msg_helper::

		//				auto ctx_ptr = std::make_shared<_Context>();
		//				ctx_ptr->attach_session(session_ptr);
		//				ctx_ptr->visit(req_ptr);
		//			});
		//	}
		//};

		// struct invoke_helper
		//{
		//	template <typename... _Args>
		//	static int invoke(uint32_t key, _Args&&... args)
		//	{
		//		std::string _key = "aquarius_" + std::to_string(key);

		//		return ctx_router::instance().invoke(_key, std::forward<_Args>(args)...);
		//	}
		//};

		// using msg_router = core::single_router<void>;

		// template <typename _Message, typename = std::is_base_of<impl::xmessage, _Message>>
		// struct msg_regist
		//{
		//	msg_regist(const std::size_t& key)
		//	{
		//		std::string _key = "aquarius_" + std::to_string(key);

		//		msg_router::instance().regist(_key, [] { return std::make_shared<_Message>(); });
		//	}
		//};
		// template <typename... _Args>
		// struct invoke_msg_helper
		//{
		//	static void invoke(uint32_t key, _Args&&... args)
		//	{
		//		std::string _key = "aquarius_" + std::to_string(key);

		//		return msg_router::instance().invoke(_key, std::forward<_Args>(args)...);
		//	}
		//};

		template <typename _Request, typename _Context>
		struct invoker
		{
			static inline bool apply(std::shared_ptr<session> trans_ptr, flex_buffer_t& buffer)
			{
				auto ctx_ptr = std::make_shared<_Context>();

				if (!ctx_ptr)
					return false;

				ctx_ptr->attach(trans_ptr);

				auto msg_ptr = std::make_shared<_Request>();

				if (!msg_ptr)
					return false;

				if (!msg_ptr->parse_message(buffer))
					return false;

				msg_ptr->accept(ctx_ptr);

				return true;
			}
		};

		using ctx_router = detail::single_router<bool, std::shared_ptr<session>, flex_buffer_t&>;

		template <typename _Request, typename _Context>
		struct ctx_regist
		{
			ctx_regist(const std::size_t& key)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				ctx_router::instance().regist(
					_key, std::bind(&invoker<_Request, _Context>::apply, std::placeholders::_1, std::placeholders::_2));
			}
		};

		struct invoke_helper
		{
			template <typename... _Args>
			static auto invoke(uint32_t key, _Args&... args)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				return ctx_router::instance().invoke(_key, args...);
			}
		};

	} // namespace impl
} // namespace aquarius