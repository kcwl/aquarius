#pragma once
#include <aquarius/channel.hpp>
#include <aquarius/core/logger.hpp>
#include <string>

namespace aquarius
{
	class service : public subscriber<subscriber_global, channel_topic, std::function<void()>>
	{
	public:
		service()
		{
			regist_start();

			regist_stop();
		}

	public:
		virtual bool init() = 0;

		virtual bool config() = 0;

		virtual bool run() = 0;

		virtual void stop() = 0;

		virtual bool enable() = 0;

		virtual std::string name() = 0;

	private:
		void regist_start()
		{
			this->subscribe(channel_topic::service_start,
							[&]
							{
								if (!enable())
									return;

								if (!config())
								{
									XLOG_ERROR() << "[servie] " << name() << " config error!";

									return;
								}

								if (!init())
								{
									XLOG_ERROR() << "[servie] " << name() << " init error!";

									return;
								}

								if (!run())
								{
									XLOG_ERROR() << "[servie] " << name() << " run error!";

									return;
								}
							});
		}

		void regist_stop()
		{
			this->subscribe(channel_topic::service_stop, [&] { this->stop(); });
		}
	};
} // namespace aquarius