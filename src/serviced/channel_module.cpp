#include "channel_module.h"
#include <aquarius/module/module_router.hpp>

namespace aquarius
{
	namespace serviced
	{
		channel_module::channel_module(const std::string& name)
			: _module<channel_module, channel_config>(name)
		{

		}

		void channel_module::subscribe(std::string_view name, std::shared_ptr<player> subscribe)
		{
			std::lock_guard lock(mutex_);

			auto& chan = channels_[name];

			if (!chan)
				chan = std::make_shared<channel>();

			chan->subscribe(subscribe);
		}
	} // namespace serviced
} // namespace aquarius

AQUARIUS_MODULE_NAMESPACE(aquarius::serviced, channel_module)