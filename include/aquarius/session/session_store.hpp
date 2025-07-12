#pragma once
#include <aquarius/config.hpp>
#include <aquarius/session/detail/basic_session_store.hpp>

namespace aquarius
{
	namespace session
	{
		template <typename Protocol>
		using container = detail::basic_store<typename Protocol::session>;

		template <typename Protocol>
		auto defer(std::size_t id)
		{
			return container<Protocol>::get_mutable_instance().find(id);
		}

		template <typename Protocol>
		void store(std::shared_ptr<typename Protocol::session> session)
		{
			return container<Protocol>::get_mutable_instance().insert(session);
		}
	} // namespace session
} // namespace aquarius