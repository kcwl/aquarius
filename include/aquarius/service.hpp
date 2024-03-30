#pragma once
#include <string>
#include <aquarius/service/service.hpp>
#include <aquarius/service/manager.hpp>

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

} // namespace aquarius

#define AQUARIUS_SERVICE_REGIST(name, service) static aquarius::service_regist<service> srv_##service(name)