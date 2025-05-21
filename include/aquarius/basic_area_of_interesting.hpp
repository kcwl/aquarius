#pragma once
#include <map>
#include <memory>

namespace aquarius
{

    template<typename Session, typename Area>
    class basic_area_of_interesting
    {
        using area_type = Area;

    public:
        basic_area_of_interesting() = default;

    public:
        template<typename Message>
        void enter(std::shared_ptr<Session> session, Message enter_msg)
        {
            {
                std::lock_guard lk(mutex_);

                auto uid = session->uid();

                auto iter = sessions_.find(uid);

                if (iter != sessions_.end())
                {
                    return;
                }

                sessions_.emplace(uid, session);
            }

            auto neighborhood = area_.enter(session);

            broadcast(neighborhood, enter_msg);
        }

        template<typename Message>
        void leave(std::size_t uid, Message leave_msg)
        {
            auto neighborhood = area_.leave(uid);

            broadcast(neighborhood, leave_msg);

            std::lock_guard lk(mutex_);

            sessions_.erase(uid);
        }

        template<typename Message, typename... Args>
        void move(std::size_t uid, Message put_msg, Message pop_msg, Args... args)
        {
            auto neighborhood = area_.move(uid, args...);

            broadcast(neighborhood, put_msg);

            broadcast(neighborhood, pop_msg);
        }

    private:
        template<typename Message>
        void broadcast(std::vector<std::size_t> neiborhood, Message msg)
        {
            for (auto& s : neiborhood)
            {
                auto neibor = sessions_[s];

                if (!neibor)
                    continue;

                neibor->send(msg);
            }
        }

    private:
        std::mutex mutex_;

        std::map<std::size_t, std::shared_ptr<Session>> sessions_;

        area_type area_;
    };
}