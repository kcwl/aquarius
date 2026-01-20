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


		auto channel_module::publish(std::string_view name, flex_buffer& buffer) ->awaitable<flex_buffer>
		{
			std::lock_guard lock(mutex_);

			auto it = channels_.find(name);

			if (it == channels_.end())
			{
				XLOG_ERROR() << "channel [" << name << "] not found";

				co_return flex_buffer{};
			}

			co_return co_await it->second->publish(buffer);
		}
	} // namespace serviced
} // namespace aquarius

AQUARIUS_MODULE_NAMESPACE(aquarius::serviced, channel_module)