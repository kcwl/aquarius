#pragma once
#include <aquarius/detail/basic_session_store.hpp>

namespace aquarius
{
    template <typename Protocol>
    using session_store = detail::basic_store<typename Protocol::session>;

    template<typename Protocol, typename T>
    std::shared_ptr<T> defer(std::size_t id)
    {
        auto session = session_store<Protocol>::get_mutable_instance().find(id);

        if (!session)
            return nullptr;

        auto result = std::dynamic_pointer_cast<T>(session);

        if (!result)
            return nullptr;

        return result;
    }

    template<typename Protocol, typename T>
    void store(std::shared_ptr<T> session)
    {
        using session_type = typename Protocol::session;

        auto id = session->id();

        auto old_session = session_store<Protocol>::get_mutable_instance().find(id);

        if (!old_session)
        {
            session_store<Protocol>::get_mutable_instance().insert(session);
        }
        else
        {
            std::shared_ptr<session_type>(session).swap(old_session);
        }
    }
}