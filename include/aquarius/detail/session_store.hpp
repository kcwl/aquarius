#pragma once
#include <aquarius/detail/basic_session_store.hpp>
#include <aquarius/detail/config.hpp>

namespace aquarius
{
	namespace detail
	{
		template <typename Session>
		using container = basic_store<Session>;

		template <typename Session>
		auto defer(std::size_t id)
		{
			return container<Session>::get_mutable_instance().find(id);
		}

		template <typename Session>
		void store(std::shared_ptr<Session> session)
		{
			return container<Session>::get_mutable_instance().insert(session);
		}
	} // namespace detail
} // namespace aquarius