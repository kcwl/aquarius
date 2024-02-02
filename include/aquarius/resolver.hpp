#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/message/invoke.hpp>
#include <aquarius/logger.hpp>


namespace aquarius
{
	template<typename _Type>
	struct resolver
	{
		static read_handle_result from_binay(flex_buffer_t& buffer, uint32_t& proto)
		{
			constexpr auto size = sizeof(uint32_t)+sizeof(uint32_t);

			if (buffer.size() < size)
				return read_handle_result::waiting_for_query;

			uint64_t proto_size{};

			buffer.sgetn((uint8_t*)&proto, sizeof(uint32_t));

			buffer.sgetn((uint8_t*)&proto_size, sizeof(uint64_t));

			if (buffer.size() < proto_size)
				return read_handle_result::waiting_for_query;

			return read_handle_result::ok;
		}
	};

	template<>
	struct resolver<http>
	{

	};
}