#pragma once
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <array>
#include <random>

namespace aquarius
{
	template <typename _Ty>
	inline _Ty hash_value(boost::uuids::uuid const& u) noexcept
	{
		_Ty seed = 0;
		for (boost::uuid::const_iterator i = u.begin(), e = u.end(); i != e; ++i)
		{
			seed ^= static_cast<_Ty>(*i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		return seed;
	}

	template<typename _Ty>
	struct uuid
	{
		static _Ty invoke()
		{
			boost::uuids::random_generator_mt19937 generator{};

			return hash_value<_Ty>(generator());
		}
	};
} // namespace aquarius