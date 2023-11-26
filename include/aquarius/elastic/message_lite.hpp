#pragma once
#include "archive.hpp"

#include <cstdint>

namespace aquarius
{
	class message_lite
	{
	public:
		message_lite() = default;

		virtual ~message_lite() = default;

	protected:
		virtual bool internal_from_binary(flex_buffer_t& buffer) = 0;

		virtual bool internal_to_binary(flex_buffer_t& buffer) = 0;

	public:
		bool to_binary(flex_buffer_t& buffer)
		{
			return internal_to_binary(buffer);
		}

		bool from_binary(flex_buffer_t& buffer)
		{
			return internal_from_binary(buffer);
		}

		bool to_json([[maybe_unused]] flex_buffer_t& buffer)
		{
			return true;
		}

		bool from_json([[maybe_unused]] flex_buffer_t& buffer)
		{
			return true;
		}

		bool to_xml([[maybe_unused]] flex_buffer_t& buffer)
		{
			return true;
		}

		bool from_xml([[maybe_unused]] flex_buffer_t& buffer)
		{
			return true;
		}
	};
} // namespace elastic