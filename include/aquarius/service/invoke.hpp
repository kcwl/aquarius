#pragma once
#include <aquarius/service/service_router.hpp>

namespace aquarius
{
	template<typename _Service>
	struct service_regist
	{
		service_regist(const std::string& service_name)
		{
			service_router::instance().regist(service_name, []() { return std::make_shared<_Service>(); });
		}
	};
}

#define AQUARIUS_SERVICE_REGIST(name, service) static aquarius::service_regist<service> srv_##service(name)