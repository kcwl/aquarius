#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/elastic.hpp>


namespace aquarius
{
	template<typename _Type>
	struct invoke_resolver_helper
	{
		static error_code from_binay(flex_buffer_t& buffer, uint32_t& proto, std::size_t& total, error_code& ec)
		{
			if (!elastic::from_binary(proto, buffer))
				return ec = system::system_errc::invalid_stream;

			if (!elastic::from_binary(total, buffer))
				return ec = system::system_errc::invalid_stream;

			if (buffer.size() < total)
				return ec = system::system_errc::wait_for_query;

			ec = error_code{};

			return ec;
		}
	};

	template<>
	struct invoke_resolver_helper<http>
	{

	};
}