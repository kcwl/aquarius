#pragma once
#include "transfer.hpp"
#include <aquarius.hpp>

namespace serviced
{
	using tcp_transfer = transfer_manager<aquarius::tcp_server_session>;
}