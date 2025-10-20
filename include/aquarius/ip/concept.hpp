#pragma once
#include <type_traits>

namespace aquarius
{
	template <typename T>
	struct is_session_type : std::false_type
	{};

	template <typename T>
	concept is_session = is_session_type<T>::value;

	template <typename T>
	struct is_ssl_socket_type : std::false_type
	{};

	template <typename T>
	concept is_ssl_socket = is_ssl_socket_type<T>::value;

	template <typename T>
	struct is_message_type : std::false_type
	{};

	template <typename T>
	concept is_message = is_message_type<T>::value;
} // namespace aquarius