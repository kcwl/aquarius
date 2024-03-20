#pragma once
#include <aquarius/router/service.hpp>

namespace aquarius
{
	template <typename _Service>
	struct service_regist
	{
		service_regist(std::size_t service_name)
		{
			service_manager::instance().regist(service_name, std::make_shared<_Service>());
		}
	};

	struct invoke_service_helper
	{
		static bool run()
		{
			return service_manager::instance().run();
		}

		static void stop()
		{
			return service_manager::instance().stop();
		}

		static bool restart_one(std::size_t key)
		{
			return service_manager::instance().restart_one(key);
		}

		static bool stop_one(std::size_t key)
		{
			return service_manager::instance().stop_one(key);
		}
	};
} // namespace aquarius

#define AQUARIUS_SERVICE_REGIST(name, service) static aquarius::service_regist<service> srv_##service(name)