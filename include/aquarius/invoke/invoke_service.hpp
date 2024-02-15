#pragma once
#include <aquarius/invoke/router_service.hpp>

namespace aquarius
{
	template<typename _Service>
	struct service_regist
	{
		service_regist(std::size_t service_name)
		{
			router_service::instance().regist(service_name, []() { return std::make_shared<_Service>(); });
		}
	};

	struct invoke_service_helper
	{
		static bool run()
		{
			return router_service::instance().run();
		}

		static void stop()
		{
			return router_service::instance().stop();
		}

		static bool restart_one(std::size_t key)
		{
			return router_service::instance().restart_one(key);
		}

		static bool stop_one(std::size_t key)
		{
			return router_service::instance().stop_one(key);
		}
	};
}

#define AQUARIUS_SERVICE_REGIST(name, service) static aquarius::service_regist<service> srv_##service(name)