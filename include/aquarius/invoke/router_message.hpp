#pragma once
#include <aquarius/system/router.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_message;
	}
} // namespace aquarius

namespace aquarius
{
	class router_message : public system::single_router<router_message, std::shared_ptr<impl::basic_message>>
	{};
} // namespace aquarius