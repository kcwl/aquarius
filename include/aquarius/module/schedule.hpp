#pragma once
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	template <typename R, typename Signature>
	struct module_task
	{
		using return_type = R;

		module_task(const Signature& func)
			: func_(func)
		{}

		template <typename T>
		auto operator()(std::shared_ptr<T> ptr) -> return_type
		{
			return func_(ptr);
		}

	private:
		Signature func_;
	};

	struct mpc
	{
		template <typename R, typename T,typename Func>
		static auto call(std::string_view module_name, Func&& f) -> awaitable<R>
		{
			auto task = std::make_shared<module_task<R, Func>>(f);

			co_return co_await module_router::get_mutable_instance().schedule<T>(std::string(module_name.data()), task);
		}
	};
} // namespace aquarius