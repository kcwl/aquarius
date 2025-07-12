#pragma once
#include "transfer.hpp"
#include <aquarius.hpp>

namespace serviced
{
	using tcp_transfer = transfer_manager<aquarius::ip::server_tcp_protocol>;
}