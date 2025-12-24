#pragma once
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	template <typename Core, typename Task>
	auto module_schedule(const std::string& module_name, std::shared_ptr<Task> task)
		-> awaitable<typename Task::return_type>
	{
		co_return co_await module_router::get_mutable_instance().schedule<Core>(module_name, task);
	}
} // namespace aquarius