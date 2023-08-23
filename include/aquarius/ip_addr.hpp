#pragma once
#include <boost/asio.hpp>
#include "detail/type_traits.hpp"

namespace aquarius
{
	template <detail::integer_t _Ty>
	std::string to_str_ip(_Ty ip)
	{
		return boost::asio::ip::address_v4(ip).to_string();
	}

	inline uint32_t to_uint_ip(const std::string& ip)
	{
		return boost::asio::ip::address_v4().from_string(ip).to_uint();
	}
}