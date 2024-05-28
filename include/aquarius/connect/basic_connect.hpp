#pragma once
#include <aquarius/core/elastic.hpp>

namespace aquarius
{
	class basic_connect
	{
	public:
		virtual void async_write(flex_buffer_t&&) = 0;

		virtual std::size_t uuid() const = 0;
	};
} // namespace aquarius