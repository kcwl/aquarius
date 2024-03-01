#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace aquarius
{
	struct uuid
	{
		static std::size_t invoke()
		{
			boost::uuids::random_generator_mt19937 generator{};

			return boost::uuids::hash_value(generator());
		}
	};
}