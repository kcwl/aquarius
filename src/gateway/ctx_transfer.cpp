#include "transfer_client.h"
#include <aquarius.hpp>

namespace gateway
{
	AQUARIUS_RAW_CONTEXT()
	{
		TRANSFER.transfer(this->message());
	}
} // namespace gateway