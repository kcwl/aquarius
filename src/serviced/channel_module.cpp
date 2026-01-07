#include "channel_module.h"

namespace aquarius
{
	namespace serviced
	{
		bool channel_module::enable()
		{
			return configs().enabled;
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