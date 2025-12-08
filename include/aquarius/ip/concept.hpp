#pragma once
#include <aquarius/asio.hpp>
#include <type_traits>

namespace aquarius
{
	template <typename T>
	struct is_session_type : std::false_type
	{};

	template <typename T>
	concept is_session = is_session_type<T>::value;

	template <typename T>
	struct is_message_type : std::false_type
	{};

	template <typename T>
	concept is_message = is_message_type<T>::value;

	template <typename T>
	struct is_ssl_socket_type : std::false_type
	{};

	template <typename T>
	concept is_ssl_socket = is_ssl_socket_type<T>::value;

	template <typename T>
	struct is_socket_type : std::false_type
	{};

	template <typename T>
	concept is_socket = is_socket_type<T>::value;

	template <typename T>
	concept is_protocol = requires {
		typename T::socket;

		typename T::resolver;

		requires is_socket<typename T::socket>;
	};

	template <typename Func>
	concept is_awaitable_func = requires(Func&& f) {
		{ f() } -> std::convertible_to<awaitable<void>>;
	};
} // namespace aquarius