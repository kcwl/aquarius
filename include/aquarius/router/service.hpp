#pragma once
#include <aquarius/router/impl/manager.hpp>
#include <aquarius/service.hpp>
#include <aquarius/logger.hpp>
#include <memory>

namespace aquarius
{
	class service_manager : public impl::single_manager<service_manager, std::shared_ptr<service>>
	{
	public:
		bool run()
		{
			int result = 1;

			for (auto& service : this->map_invokes_)
			{
				if (!service.second->enable())
					continue;

				auto res = start_one(service.second);

				result &= static_cast<int>(res);
			}

			return result != 0;
		}

		void stop()
		{
			for (auto& service : this->map_invokes_)
			{
				if (!service.second->enable())
					continue;

				service.second->stop();
			}
		}

		bool restart_one(std::size_t key)
		{
			auto service = this->invoke(key);

			if (!service)
				return false;

			return start_one(service);
		}

		bool stop_one(std::size_t key)
		{
			auto service = this->invoke(key);

			if (!service)
				return false;

			service->stop();

			return true;
		}

	private:
		bool start_one(std::shared_ptr<service> ptr)
		{
			if (!ptr->config())
			{
				XLOG_ERROR() << "[servie] " << ptr->name() << " config error!";

				return false;
			}
				
			if (!ptr->init())
			{
				XLOG_ERROR() << "[servie] " << ptr->name() << " init error!";

				return false;
			}
				

			if (!ptr->run())
			{
				XLOG_ERROR() << "[servie] " << ptr->name() << " run error!";

				return false;
			}

			return true;
		}
	};
} // namespace aquarius