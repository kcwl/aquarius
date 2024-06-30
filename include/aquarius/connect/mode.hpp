#pragma once

namespace aquarius
{
	struct http
	{};

	struct tcp
	{};

	constexpr static auto pack_limit = 4096;

	enum class pack_flag
	{
		normal = 0x01,
		middle = 0x10,
		end = 0x11
	};
} // namespace aquarius