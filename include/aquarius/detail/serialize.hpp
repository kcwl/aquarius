#pragma once

namespace aquarius
{
	template <typename T, typename Buffer>
	bool from_binary(T request, Buffer& buffer)
	{
		return request->from_binary(buffer);
	}
} // namespace aquarius