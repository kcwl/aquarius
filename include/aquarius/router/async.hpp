#pragma once
#include <aquarius/router/impl/apply.hpp>
#include <aquarius/router/impl/generator.hpp>

namespace aquarius
{
	class basic_message;
} // namespace aquarius

namespace aquarius
{
	class async_generator : public impl::single_generator<async_generator, bool, std::shared_ptr<basic_message>>
	{};
} // namespace aquarius