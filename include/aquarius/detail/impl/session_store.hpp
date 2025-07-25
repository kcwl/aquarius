#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/impl/basic_session_store.hpp>

namespace aquarius
{
	namespace detail
	{
		namespace impl
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
		} // namespace impl
	} // namespace detail
} // namespace aquarius