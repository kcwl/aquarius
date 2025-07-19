#include "transfer_client.h"
#include <aquarius/context.hpp>

namespace gateway
{
	AQUARIUS_TRANSFER_CONTEXT()
	{
		TRANSFER.async_send(this->buffer());
	}
} // namespace gateway