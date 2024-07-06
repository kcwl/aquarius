#pragma once
#include <cstddef>
#include <aquarius/core/uuid.hpp>

namespace aquarius
{
	class role
	{
	public:
		role()
			: uuid_(invoke_uuid<std::size_t>())
		{

		}

	public:
		std::size_t uuid() const
		{
			return uuid_;
		}

	public:
		std::size_t uuid_;
	};
} // namespace aquarius