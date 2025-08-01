#pragma once
#include <aquarius/detail/config.hpp>
#include <unordered_map>
#include <functional>
#include <aquarius/noncopyable.hpp>
#include <aquarius/singleton.hpp>


namespace aquarius
{
    template <typename Key, typename Value>
    class router_base : public noncopyable
    {
    public:
        router_base() = default;

        virtual ~router_base() = default;

    protected:
        std::unordered_map<Key, Value> map_invokes_;
    };

    template <typename Result, typename... Args>
    class basic_router : public router_base<std::size_t, std::function<Result(Args...)>>
    {
    public:
        basic_router() = default;

    public:
        Result invoke(std::size_t key, Args... args)
        {
            auto iter = this->map_invokes_.find(key);

            if (iter == this->map_invokes_.end())
                return Result();

            return iter->second(args...);
        }
    };

    template <typename T, typename Result, typename... Args>
    class single_router : public basic_router<Result, Args...>, public singleton<T>
    {
    };

} // namespace aquarius
