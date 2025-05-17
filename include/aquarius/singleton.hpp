#pragma once
#include <boost/serialization/singleton.hpp>

namespace aquarius
{
	template <typename T>
	using singleton = boost::serialization::singleton<T>;
} // namespace aquarius
