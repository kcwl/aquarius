#pragma once
#include <aquarius/core/generator.hpp>
#include <memory>

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class async_generator : public single_generator<async_generator, bool, std::shared_ptr<basic_message>>
	{};
} // namespace aquarius