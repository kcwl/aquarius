#pragma once
#include <aquarius/basic_level.hpp>

namespace aquarius
{

    template<std::size_t Step, std::size_t Level>
    class basic_area;

    template<std::size_t Step>
    class basic_area<Step, 1>
    {
    public:
        template<typename... Args>
        void append(std::size_t id, Args&&... args)
        {
            static_assert(sizeof...(args) > 0, "param number error");

            x_.append(id, std::get<0>(args...));
        }

        void remove(std::size_t id)
        {
            x_.remove(id);
        }


        template<typename... Args>
        void update(std::size_t id, Args&&... args)
        {
            static_assert(sizeof...(args) > 0, "param number error");

            x_.update(id, std::get<0>(args...));
        }

    private:
        level<Step> x_;
    };

    template<std::size_t Step>
    class basic_area<Step, 2>
    {
    public:
        template<typename... Args>
        void append(std::size_t id, Args&&... args)
        {
            static_assert(sizeof...(args) > 1, "param number error");

            x_.append(id, std::get<0>(args...));
            y_.append(id, std::get<1>(args...));
        }

        void remove(std::size_t id)
        {
            x_.remove(id);
            y_.remove(id);
        }


        template<typename... Args>
        void update(std::size_t id, Args&&... args)
        {
            static_assert(sizeof...(args) > 0, "param number error");

            x_.update(id, std::get<0>(args...));
        }

    private:
        level<Step> x_;
        level<Step> y_;
    };

    template<std::size_t Step>
    class basic_area<Step, 3>
    {
    public:
    private:
        level<Step> x_;
        level<Step> y_;
        level<Step> z_;
    };

}