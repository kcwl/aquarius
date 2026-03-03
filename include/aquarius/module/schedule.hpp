#pragma once
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	template <typename R, typename Signature>
	struct module_data
	{
		using return_type = R;

		module_data(const Signature& func)
			: func_(func)
		{}

		template <typename T>
		auto operator()(T* ptr) -> awaitable<return_type>
		{
			co_return co_await func_(ptr);
		}

	private:
		Signature func_;
	};

	template <typename R, typename Signature>
	struct module_sync_data
	{
		using return_type = R;

		module_sync_data(const Signature& f)
			: func(f)
		{}

		template <typename T>
		auto operator()(T* ptr)
		{
			return func(ptr);
		}

		Signature func;
	};

	struct mpc
	{
		template <typename R, typename T, typename Func>
		static auto call(Func&& f) -> awaitable<R>
		{
			auto task = std::make_shared<module_data<R, Func>>(f);

			co_return co_await module_router::get_mutable_instance().schedule<T>(task);
		}

		template <typename R, typename T, typename Func>
		static auto call_sync(Func&& f) -> R
		{
			auto task = std::make_shared<module_sync_data<R, Func>>(f);

			return module_router::get_mutable_instance().sync_schedule<T>(task);
		}
	};

} // namespace aquarius