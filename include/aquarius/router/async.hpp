#pragma once
#include <aquarius/router/impl/generator.hpp>
#include <aquarius/router/impl/apply.hpp>

namespace aquarius
{
	namespace impl
	{
		class basic_message;
	}
} // namespace aquarius

namespace aquarius
{
	class async_generator : public impl::single_generator<async_generator, bool, std::shared_ptr<impl::basic_message>>
	{};
} // namespace aquarius