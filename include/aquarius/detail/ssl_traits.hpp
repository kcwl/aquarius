#pragma once
#include <boost/system/error_code.hpp>
#include <concepts>

namespace aquarius
{
#ifdef AQUARIUS_ENABLE_SSL
	struct ssl
	{
		static constexpr auto version = boost::asio::ssl::context_base::sslv23;
	};
#else
	struct ssl
	{
		static constexpr std::size_t version = static_cast<std::size_t>(-1);
	};
#endif

	template <typename T>
	concept ssl_mode = !std::is_same_v<std::decay_t<T>, void>;

	template <typename Socket, typename SSLContext>
	class null_ssl
	{
	public:
		explicit null_ssl(Socket&, SSLContext) {};

		void shutdown(boost::system::error_code)
		{}

		template <typename... Args>
		auto async_handshake(Args&&... args) -> boost::asio::awaitable<void>
		{
			co_return;
		}
	};

	template <typename T>
	struct ssl_version_traits
	{
		static constexpr std::size_t value = static_cast<std::size_t>(-1);
	};

	template <>
	struct ssl_version_traits<ssl>
	{
		static constexpr auto value = ssl::version;
	};
} // namespace aquarius