#pragma once
#include <aquarius/core/generator.hpp>

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class message_generator : public single_generator<message_generator, std::shared_ptr<basic_message>>
	{};
} // namespace aquarius