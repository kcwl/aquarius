#pragma once
#include <aquarius/context.hpp>
#include <aquarius/detail/router.hpp>
#include <aquarius/impl/flex_buffer.hpp>

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

				if (!msg_ptr->visit(&buffer, visit_mode::input))
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