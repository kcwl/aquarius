#pragma once
#include <boost/uuid.hpp>

namespace aquarius
{
	namespace detail
	{
		struct uuid_generator
		{
			std::size_t operator()()
			{
				boost::uuids::random_generator gen;

				return boost::uuids::hash_value(gen());
			}
		};
	} // namespace detail
} // namespace aquarius