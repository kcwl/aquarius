#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/message/invoke.hpp>
#include <elastic.hpp>


namespace aquarius
{
	template<typename _Type>
	struct resolver
	{
		read_handle_result from_binay(flex_buffer_t& buffer, uint32_t proto)
		{
			constexpr auto prev_header_size = sizeof(uint32_t) + sizeof(uint32_t);

			if (buffer.size() < prev_header_size)
				return read_handle_result::error;

			if (!elastic::from_binary(proto, buffer))
				return read_handle_result::unknown_proto;

			uint32_t proto_size{};

			if (!elastic::from_binary(proto_size, buffer))
				return read_handle_result::null_message;

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