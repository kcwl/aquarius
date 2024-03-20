#pragma once
#include <aquarius/router/impl/manager.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_message;
	}
} // namespace aquarius

namespace aquarius
{
	class message_generator : public impl::single_generator<message_generator, std::shared_ptr<impl::basic_message>>
	{};
} // namespace aquarius