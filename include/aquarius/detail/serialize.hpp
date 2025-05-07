#pragma once

namespace aquarius
{
	template <typename Request, typename Buffer>
	bool from_binary(std::shared_ptr<Request> request, Buffer& buffer)
	{
		return request->from_binary(buffer);
	}
} // namespace aquarius