#pragma once
#include <boost/asio/error.hpp>
#include <system_error>

namespace aquarius
{
	namespace impl
	{
		struct aquarius_error
		{
			static boost::system::error_code& get()
			{
				thread_local static boost::system::error_code last_error_{};

				return last_error_;
			}
		};

		inline boost::system::error_code& get_last_error() noexcept
		{
			return aquarius_error::get();
		}

		template <typename _Error>
		inline void set_last_error(_Error e) noexcept
		{
			using type = std::remove_cvref_t<_Error>;

			if constexpr (std::same_as<type, boost::asio::error::basic_errors> ||
						  std::same_as<type, boost::asio::error::netdb_errors> ||
						  std::same_as<type, boost::asio::error::addrinfo_errors> ||
						  std::same_as<type, boost::asio::error::misc_errors>)
			{
				get_last_error() = boost::asio::error::make_error_code(e);
			}
			else if constexpr (std::same_as<type, std::errc> || std::same_as<type, std::io_errc>)
			{
				get_last_error() = std::make_error_code(e);
			}
			else if constexpr (std::same_as<type, boost::system::error_code>)
			{
				get_last_error() = e;
			}
			else
			{
				get_last_error().assign(static_cast<int>(e), boost::asio::error::get_system_category());
			}
		}
	} // namespace impl
} // namespace aquarius