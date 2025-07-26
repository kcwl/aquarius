#pragma once
#include <aquarius/traits/async.hpp>

namespace aquarius
{
	template<typename T>
	concept client_t = is_client<T>::value;
}