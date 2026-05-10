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
	inline auto mpc_async_call(Args&&... args) -> typename member_func_pointer<decltype(MemberFunc)>::return_type
	{
		using return_type = typename member_func_pointer<decltype(MemberFunc)>::return_type::value_type;
		using class_type = typename member_func_pointer<decltype(MemberFunc)>::class_type;

		auto f = [tp = std::move(std::make_tuple(std::forward_like<Args>(args)...))](
					 class_type* m) mutable -> asio::awaitable<return_type>
		{ return std::apply(std::bind_front(MemberFunc, m), std::move(tp)); };

		auto executor = module_router<>::get_mutable_instance().get_executor();

		co_return co_await asio::co_spawn(
			executor,
			[func = std::move(f)] () mutable -> asio::awaitable<return_type>
			{
				co_return co_await module_router<>::get_mutable_instance().async_schedule<class_type, return_type>(
					/*task*/ func);
			},
			asio::use_awaitable);
	}

	template <auto MemberFunc, typename... Args>
	inline auto mpc_call(Args&&... args)
		-> asio::awaitable<typename member_func_pointer<decltype(MemberFunc)>::return_type>
	{
		using return_type = typename member_func_pointer<decltype(MemberFunc)>::return_type;
		using class_type = typename member_func_pointer<decltype(MemberFunc)>::class_type;

		auto f = [tp = std::move(std::make_tuple(std::forward_like<Args>(args)...))](
					 class_type* m) mutable -> asio::awaitable<return_type>
		{ co_return std::apply(std::bind_front(MemberFunc, m), tp); };

		auto executor = module_router<>::get_mutable_instance().get_executor();

		co_return co_await asio::co_spawn(
			executor,
			[func = std::move(f)]() -> asio::awaitable<return_type>
			{
				co_return module_router<>::get_mutable_instance().schedule<class_type, return_type>(
					/*task*/ std::move(func));
			},
			asio::use_awaitable);
	}

} // namespace aquarius