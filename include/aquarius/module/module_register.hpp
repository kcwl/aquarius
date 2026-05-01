#pragma once
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	template <typename Module>
	struct auto_module_register
	{
		auto_module_register()
		{
			module_router::get_mutable_instance().regist<Module>();
		}
	};
} // namespace aquarius

#define AQUARIUS_MODULE(m)                                                                                             \
	class m;                                                                                                           \
	[[maybe_unused]] inline static aquarius::auto_module_register<m> __auto_register_##m;                              \
	class m : public basic_module<m>