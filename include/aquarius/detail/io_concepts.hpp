#pragma once
#include <concepts>

namespace boost
{
	namespace asio
	{
		class execution_context;
	}
}

namespace aquarius
{
	template<typename T>
	concept execution_context_convertible = std::is_convertible_v<T&, boost::asio::execution_context&>;
}