#pragma once
#include <aquarius/channel/subscriber.hpp>
#include <aquarius/channel/topic.hpp>
#include <aquarius/core/logger.hpp>
#include <aquarius/core/manager.hpp>
#include <aquarius/service/service.hpp>
#include <memory>

namespace aquarius
{
	class service_manager : public single_manager<service_manager, std::shared_ptr<service>>
	{};
} // namespace aquarius