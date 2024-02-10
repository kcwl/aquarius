#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/elastic.hpp>


namespace aquarius
{
	template<typename _Type>
	struct resolver
	{
		static read_handle_result from_binay(flex_buffer_t& buffer, uint32_t& proto, std::size_t& total)
		{
			if (!elastic::from_binary(proto, buffer))
				return read_handle_result::unknown_error;

			if (!elastic::from_binary(total, buffer))
				return read_handle_result::unknown_error;

			if (buffer.size() < total)
				return read_handle_result::waiting_for_query;

			return read_handle_result::ok;
		}
	};

	template<>
	struct resolver<http>
	{

	};
}