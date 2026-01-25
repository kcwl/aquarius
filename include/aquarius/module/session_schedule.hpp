#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/module/schedule.hpp>
#include <aquarius/module/session_module.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	constexpr static auto session_module_name = "session_module"sv;

	template <typename Session>
	inline auto mpc_insert_session(std::shared_ptr<Session> session_ptr) -> awaitable<bool>
	{
		co_return co_await mpc::call<bool, session_module<Session>>(session_module_name,
																	[&](session_module<Session>* ptr) -> awaitable<bool>
																	{ co_return ptr->insert(session_ptr); });
	}

	template<typename Session>
	inline auto mpc_erase_session(std::size_t id) -> awaitable<void>
	{
		co_return co_await mpc::call<void, session_module<Session>>(
			session_module_name, [&](session_module<Session>* ptr) -> awaitable<void> { co_return ptr->erase(id); });
	}

	template <typename Session>
	inline auto mpc_find_session(std::size_t id) -> awaitable<std::shared_ptr<Session>>
	{
		co_return co_await mpc::call<std::shared_ptr<Session>, session_module<Session>>(
			session_module_name,
			[&](session_module<Session>* ptr) -> awaitable<std::shared_ptr<Session>> { co_return ptr->find(id); });
	}
} // namespace aquarius