#pragma once
#include <concepts>

namespace boost
{
	namespace asio
	{
		class execution_context;
	}
} // namespace boost

namespace aquarius
{
	class execution_context;

	template <typename T>
	concept execution_context_convertible = std::is_convertible_v<T&, execution_context&>;
} // namespace aquarius