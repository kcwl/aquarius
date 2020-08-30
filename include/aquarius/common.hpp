#pragma once


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
}