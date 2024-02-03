#pragma once
#include <aquarius/invoke/router_service.hpp>

namespace aquarius
{
	template<typename _Service>
	struct service_regist
	{
		service_regist(std::size_t service_name)
		{
			service_router::instance().regist(service_name, []() { return std::make_shared<_Service>(); });
		}
	};

	struct service_invoke_helper
	{
		static bool run()
		{
			return service_router::instance().run();
		}

		static void stop()
		{
			return service_router::instance().stop();
		}

		static bool restart_one(std::size_t key)
		{
			return service_router::instance().restart_one(key);
		}

		static bool stop_one(std::size_t key)
		{
			return service_router::instance().stop_one(key);
		}
	};
}

#define AQUARIUS_SERVICE_REGIST(name, service) static aquarius::service_regist<service> srv_##service(name)