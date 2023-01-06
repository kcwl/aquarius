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
		using ctx_router = core::single_router<int, std::shared_ptr<impl::session>, std::shared_ptr<impl::xmessage>>;

		template <typename _Context>
		struct ctx_regist
		{
			ctx_regist(const std::size_t& key)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				ctx_router::instance().regist(
					_key, [](std::shared_ptr<impl::session> session_ptr, std::shared_ptr<impl::xmessage> req_ptr)
					{ std::make_shared<_Context>(session_ptr)->visit(req_ptr); });
			}
		};

		struct invoke_helper
		{
			template <typename... _Args>
			static int invoke(uint32_t key, _Args&&... args)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				return ctx_router::instance().invoke(_key, std::forward<_Args>(args)...);
			}
		};

		using msg_router = core::single_router<std::shared_ptr<impl::xmessage>>;

		template <typename _Message, typename = std::is_base_of<impl::xmessage, _Message>>
		struct msg_regist
		{
			msg_regist(const std::size_t& key)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				msg_router::instance().regist(_key, [] { return std::make_shared<_Message>(); });
			}
		};
		template <typename _Message, typename... _Args>
		struct invoke_msg_helper
		{
			static _Message invoke(uint32_t key, _Args&&... args)
			{
				std::string _key = "aquarius_" + std::to_string(key);

				return msg_router::instance().invoke(_key, std::forward<_Args>(args)...);
			}
		};
	} // namespace impl
} // namespace aquarius