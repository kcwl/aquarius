#include "error.hpp"
#include <aquarius.hpp>
#include <cmd.proto.hpp>

namespace srdh
{
	AQUARIUS_STREAM_RESPONSE(cmd_resp, rpc_cmd)
	{
		std::cout << this->request()->body().output << std::endl;

		std::cout << "serviced-ssh> ";

		co_return errc::success;
	}
} // namespace srdh