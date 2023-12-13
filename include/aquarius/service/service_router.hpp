#pragma once
#include <aquarius/detail/router.hpp>
#include <aquarius/service/service.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace aquarius
{
	class service_router : public detail::single_router<service_router, std::shared_ptr<service>>
	{
	public:
		bool run()
		{
			for (auto& invoke : this->map_invokes_)
			{
				services_.insert({ invoke.first, invoke.second() });
			}

			for (auto& service : services_)
			{
				if (!service.second)
					continue;

				if (!service.second->enable())
					continue;

				start_one(service.second);
			}

			return true;
		}

		void stop()
		{
			for (auto& service : services_)
			{
				if (!service.second)
					continue;

				if (!service.second->enable())
					continue;

				service.second->stop();
			}
		}

		bool restart_one(const std::string& key)
		{
			auto iter = services_.find(key);

			if (iter == services_.end())
				return false;

			return start_one(iter->second);
		}

		bool stop_one(const std::string& key)
		{
			auto iter = services_.find(key);

			if (iter == services_.end())
				return false;

			if (!iter->second)
				return false;

			iter->second->stop();

			return true;
		}

	private:
		bool start_one(std::shared_ptr<service> ptr)
		{
			if (!ptr)
				return false;

			if (!ptr->init())
				return false;

			if (!ptr->config())
				return false;

			if (!ptr->run())
				return false;

			return true;
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<service>> services_;
	};
} // namespace aquarius