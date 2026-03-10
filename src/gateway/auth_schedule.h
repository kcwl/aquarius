#pragma once
#include <aquarius/asio.hpp>
#include <memory>
#include <string>

namespace aquarius
{
	namespace gateway
	{
		auto mpc_auth(const std::string& user, const std::string& passwd) -> awaitable<bool>;

	} // namespace gateway
} // namespace aquarius