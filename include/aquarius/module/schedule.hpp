#pragma once
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	template <typename T>
	struct member_func_pointer;

	template <typename R, typename T, typename... Args>
	struct member_func_pointer<R (T::*)(Args...)>
	{
		using return_type = R;
		using class_type = T;
	};
	template <typename R, typename T, typename... Args>
	struct member_func_pointer<R (T::*)(Args...) const>
	{
		using return_type = R;
		using class_type = T;
	};

	template <auto MemberFunc, typename... Args>
	inline auto mpc_async_call(Args&&... args)
		-> typename member_func_pointer<decltype(MemberFunc)>::return_type
	{
		using return_type = typename member_func_pointer<decltype(MemberFunc)>::return_type::value_type;
		using class_type = typename member_func_pointer<decltype(MemberFunc)>::class_type;

		auto f = [&](class_type* m) -> asio::awaitable<return_type>
		{
			auto func = std::bind(MemberFunc, m, std::forward<Args>(args)...);

			co_return co_await func();
		};

		co_return co_await module_router::get_mutable_instance().async_schedule<class_type, return_type>(/*task*/ f);
	}

	template <auto MemberFunc, typename... Args>
	inline auto mpc_call(Args&&... args) -> typename member_func_pointer<decltype(MemberFunc)>::return_type
	{
		using return_type = typename member_func_pointer<decltype(MemberFunc)>::return_type;
		using class_type = typename member_func_pointer<decltype(MemberFunc)>::class_type;

		auto f = [&](class_type* m)
		{
			auto func = std::bind(MemberFunc, m, std::forward<Args>(args)...);

			return func();
		};

		return module_router::get_mutable_instance().schedule<class_type, return_type>(/*task*/ f);
	}

} // namespace aquarius