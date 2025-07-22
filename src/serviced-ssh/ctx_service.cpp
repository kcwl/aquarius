#include "error.hpp"
#include <aquarius.hpp>
#include <cmd.proto.hpp>

namespace srdh
{
	AQUARIUS_RESPONSE(cmd_resp, rpc_cmd)
	{
		std::cout << this->message()->body().output;

		std::cout << "serviced-ssh> ";

		co_return errc::success;
	}
} // namespace srdh