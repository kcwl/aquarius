#pragma once
#include <aquarius/system/router.hpp>
#include <aquarius/service.hpp>
#include <memory>

namespace aquarius
{
	class router_service : public system::single_router<router_service, std::shared_ptr<service>>
	{
	public:
		bool run()
		{
			for (auto& invoke : this->map_invokes_)
			{
				services_.insert({ invoke.first, invoke.second() });
			}

			int result = 1;

			for (auto& service : services_)
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
			for (auto& service : services_)
			{
				if (!service.second->enable())
					continue;

				service.second->stop();
			}
		}

		bool restart_one(std::size_t key)
		{
			auto iter = services_.find(key);

			if (iter == services_.end())
				return false;

			return start_one(iter->second);
		}

		bool stop_one(std::size_t key)
		{
			auto iter = services_.find(key);

			if (iter == services_.end())
				return false;

			iter->second->stop();

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

	private:
		std::unordered_map<std::size_t, std::shared_ptr<service>> services_;
	};
} // namespace aquarius