#pragma once
#include <concepts>
#include <string>
#include <string_view>

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

	template<typename T>
	struct is_string : std::false_type{};

	template<>
	struct is_string<const char*> : std::true_type {};

	template<std::size_t N>
	struct is_string<const char[N]> : std::true_type {};

	template<>
	struct is_string<char*> : std::true_type {};

	template<>
	struct is_string<std::string> : std::true_type {};

	template<>
	struct is_string<std::string_view> : std::true_type {};
} // namespace aquarius