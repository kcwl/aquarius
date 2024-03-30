#pragma once
#include <aquarius/core/generator.hpp>

namespace aquarius
{
	class basic_context;
} // namespace aquarius

namespace aquarius
{
	class context_generator : public single_generator<context_generator, std::shared_ptr<basic_context>>
	{};
} // namespace aquarius