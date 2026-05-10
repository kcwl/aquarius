#pragma once
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	template <typename Module>
	struct auto_module_register
	{
		auto_module_register(int priority = 0)
		{
			module_router<>::get_mutable_instance().regist<Module>(priority);
		}
	};
} // namespace aquarius

#define AQUARIUS_MODULE(m)                                                                                             \
	class m;                                                                                                           \
	[[maybe_unused]] inline static aquarius::auto_module_register<m> __auto_register_##m;                              \
	class m : public basic_module<m>

#define AQUARIUS_MODULE_PRI(m, p)                                                                                      \
	class m;                                                                                                           \
	[[maybe_unused]] inline static aquarius::auto_module_register<m> __auto_register_##m(p);                           \
	class m : public basic_module<m>


#define AQUARIUS_MODULE_TOP(m) AQUARIUS_MODULE_PRI(m, std::numeric_limits<int>::max())