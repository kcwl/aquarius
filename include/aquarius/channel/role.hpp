#pragma once
#include <cstddef>

namespace aquarius
{
	template <typename _Topic>
	class role
	{
	public:
		std::size_t uuid() const
		{
			return uuid_;
		}

	public:
		std::size_t uuid_;
	};
} // namespace aquarius