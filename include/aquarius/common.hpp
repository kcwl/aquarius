﻿#pragma once
#include "detail/easybuffers/include/easybuffers.hpp"

namespace aquarius
{
	enum class MessageLength
	{
		max_message_length = 8 * 4 + 4 * 1024,
		max_message_head_length = 4 * 5,
		max_message_body_length = 4 * 1024
	};

	enum class Protocol : int
	{
		Tcp,
		Http
	};

	enum class ConnectState : int
	{
		connecting,
		shutdown
	};

	using streambuf = easybuffers::ebstream<>;

	using protocol_type = uint32_t;
}