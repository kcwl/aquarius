#pragma once
#include <boost/uuid.hpp>

namespace aquarius
{
	namespace detail
	{
		struct uuid_generator
		{
			uint32_t operator()()
			{
				boost::uuids::random_generator gen;

				return static_cast<uint32_t>(boost::uuids::hash_value(gen()));
			}
		};
	} // namespace detail
} // namespace aquarius