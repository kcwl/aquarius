#pragma once
#include <aquarius/detail/tag.hpp>
#include <aquarius/sql/database_param.hpp>

namespace aquarius
{
	namespace detail
	{
		struct io_context_param;
	}
}

namespace aquarius
{
	template<typename T>
	void tag_invoke(value_to<T>, database_param&)
	{
		return;
	}

	template<typename T>
	void tag_invoke(value_to<T>, detail::io_context_param&)
	{
		return;
	}
} // namespace aquarius
