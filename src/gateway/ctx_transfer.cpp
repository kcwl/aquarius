#include "transfer_client.h"
#include <aquarius.hpp>

namespace gateway
{
	AQUARIUS_RAW_CONTEXT()
	{
		TRANSFER.async_send(this->message());
	}
} // namespace gateway